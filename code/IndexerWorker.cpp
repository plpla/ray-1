/*
 	Ray
    Copyright (C) 2010, 2011  Sébastien Boisvert

	http://DeNovoAssembler.SourceForge.Net/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You have received a copy of the GNU General Public License
    along with this program (COPYING).  
	see <http://www.gnu.org/licenses/>

*/

#include <IndexerWorker.h>

void IndexerWorker::constructor(int sequenceId,char*sequence,Parameters*parameters,RingAllocator*outboxAllocator,
	VirtualCommunicator*vc,uint64_t workerId){
	m_sequenceId=sequenceId;
	m_sequence=sequence;
	m_parameters=parameters;
	m_outboxAllocator=outboxAllocator;
	m_virtualCommunicator=vc;
	m_workerId=workerId;
	m_done=false;
	m_forwardIndexed=false;
	m_reverseIndexed=false;
	m_position=0;
	m_coverageRequested=false;
	m_theLength=strlen(sequence);
	m_vertexIsDone=false;
	m_vertexInitiated=false;
	m_fetchedCoverageValues=false;
}

bool IndexerWorker::isDone(){
	return m_done;
}

void IndexerWorker::work(){
	if(m_done){
		return;
	}else if(!m_fetchedCoverageValues){
		if(m_position>m_theLength-m_parameters->getWordSize()){
			m_fetchedCoverageValues=true;
		}else if(!m_coverageRequested){
			//cout<<"Seq ="<<m_sequence<<" "<<m_parameters->getWordSize()endl;
			uint64_t vertex=kmerAtPosition(m_sequence.c_str(),m_position,m_parameters->getWordSize(),'F',m_parameters->getColorSpaceMode());
			m_vertices.push_back(vertex);
			int sendTo=vertexRank(vertex,m_parameters->getSize(),m_parameters->getWordSize());
			uint64_t*message=(uint64_t*)m_outboxAllocator->allocate(1*sizeof(uint64_t));
			message[0]=vertex;
			Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,sendTo,RAY_MPI_TAG_REQUEST_VERTEX_COVERAGE,m_parameters->getRank());
			m_virtualCommunicator->pushMessage(m_workerId,&aMessage);
			m_coverageRequested=true;
		}else if(m_virtualCommunicator->isMessageProcessed(m_workerId)){
			int coverage=m_virtualCommunicator->getResponseElements(m_workerId)[0];
			m_coverages.push_back(coverage);
			//cout<<"Coverage="<<coverage<<endl;
			m_position++;
			m_coverageRequested=false;
		}
	}else if(!m_forwardIndexed){
		if(!m_vertexIsDone){
			int selectedPosition=-1;
			// find a vertex that is not an error and that is not repeated
			for(int i=0;i<(int)m_coverages.size()/2;i++){
				int coverage=m_coverages[i];
				if(coverage>=m_parameters->getMinimumCoverage()/2&&coverage<m_parameters->getPeakCoverage()*2){
					selectedPosition=i;
					break;
				}
			}

			// find a vertex that is not an error 
			if(selectedPosition==-1){
				for(int i=0;i<(int)m_coverages.size();i++){
					int coverage=m_coverages[i];
					if(coverage>=m_parameters->getMinimumCoverage()/2){
						selectedPosition=i;
						break;
					}
				}
	
			}
/*
			cout<<"Seq="<<m_sequence<<endl;
			cout<<" Coverages ";
			for(int i=0;i<(int)m_coverages.size();i++){
				cout<<" "<<i<<":"<<m_coverages[i];
			}
			cout<<" ForwardSelection="<<selectedPosition<<endl;
*/
			// index it
			if(selectedPosition!=-1){
				uint64_t vertex=m_vertices[selectedPosition];
				int sendTo=vertexRank(vertex,m_parameters->getSize(),m_parameters->getWordSize());
				uint64_t*message=(uint64_t*)m_outboxAllocator->allocate(5*sizeof(uint64_t));
				message[0]=vertex;
				message[1]=m_parameters->getRank();
				message[2]=m_sequenceId;
				message[3]=selectedPosition;
				message[4]='F';
				Message aMessage(message,5,MPI_UNSIGNED_LONG_LONG,sendTo,RAY_MPI_TAG_ATTACH_SEQUENCE,m_parameters->getRank());
				m_virtualCommunicator->pushMessage(m_workerId,&aMessage);
				m_vertexIsDone=true;
			}else{
				m_forwardIndexed=true;
			}
		}else if(m_virtualCommunicator->isMessageProcessed(m_workerId)){
			m_virtualCommunicator->getResponseElements(m_workerId);
			m_forwardIndexed=true;
			m_reverseIndexed=false;
			m_vertexIsDone=false;
		}
	}else if(!m_reverseIndexed){
		if(!m_vertexIsDone){
			int selectedPosition=-1;
			// find a vertex that is not an error and that is not repeated
			for(int i=(int)m_coverages.size()-1;i>=(int)m_coverages.size()/2;i--){
				int coverage=m_coverages[i];
				if(coverage>=m_parameters->getMinimumCoverage()/2&&coverage<m_parameters->getPeakCoverage()*2){
					selectedPosition=i;
					break;
				}
			}

			// find a vertex that is not an error 
			if(selectedPosition==-1){
				for(int i=(int)m_coverages.size()-1;i>=0;i--){
					int coverage=m_coverages[i];
					if(coverage>=m_parameters->getMinimumCoverage()/2){
						selectedPosition=i;
						break;
					}
				}
	
			}
/*
			cout<<"Seq="<<m_sequence<<endl;
			cout<<" Coverages ";
			for(int i=0;i<(int)m_coverages.size();i++){
				cout<<" "<<i<<":"<<m_coverages[i];
			}
			cout<<" ReverseSelection="<<selectedPosition<<endl;
*/

			// index it
			if(selectedPosition!=-1){
				uint64_t vertex=m_parameters->_complementVertex(m_vertices[selectedPosition]);
				int sendTo=vertexRank(vertex,m_parameters->getSize(),m_parameters->getWordSize());
				uint64_t*message=(uint64_t*)m_outboxAllocator->allocate(5*sizeof(uint64_t));
				message[0]=vertex;
				message[1]=m_parameters->getRank();
				message[2]=m_sequenceId;
				message[3]=m_theLength-m_parameters->getWordSize()-selectedPosition;
				message[4]='R';
				Message aMessage(message,5,MPI_UNSIGNED_LONG_LONG,sendTo,RAY_MPI_TAG_ATTACH_SEQUENCE,m_parameters->getRank());
				m_virtualCommunicator->pushMessage(m_workerId,&aMessage);
				m_vertexIsDone=true;
			}else{
				m_reverseIndexed=true;
			}
		}else if(m_virtualCommunicator->isMessageProcessed(m_workerId)){
			m_virtualCommunicator->getResponseElements(m_workerId);
			m_reverseIndexed=true;
		}

	}else{
		m_done=true;
	}
}

uint64_t IndexerWorker::getWorkerId(){
	return m_workerId;
}


