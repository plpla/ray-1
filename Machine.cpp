/*
 	Ray
    Copyright (C) 2010  Sébastien Boisvert

	http://DeNovoAssembler.SourceForge.Net/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You have received a copy of the GNU General Public License
    along with this program (gpl-3.0.txt).  
	see <http://www.gnu.org/licenses/>

*/

#define MAX_DEPTH 200
#define MAX_VERTICES_TO_VISIT 500
#define TIP_LIMIT 40
#define _MINIMUM_COVERAGE 2


// tags
// these are the message types used by Ray
// Ray instances like to communicate a lots!
#define TAG_WELCOME 0
#define TAG_SEND_SEQUENCE 1
#define TAG_SEQUENCES_READY 2
#define TAG_MASTER_IS_DONE_SENDING_ITS_SEQUENCES_TO_OTHERS 3
#define TAG_VERTICES_DATA 4
#define TAG_VERTICES_DISTRIBUTED 5
#define TAG_VERTEX_PTR_REQUEST 6
#define TAG_OUT_EDGE_DATA_WITH_PTR 7
#define TAG_OUT_EDGES_DATA 8
#define TAG_SHOW_VERTICES 9
#define TAG_START_VERTICES_DISTRIBUTION 10
#define TAG_EDGES_DISTRIBUTED 11
#define TAG_IN_EDGES_DATA 12
#define TAG_IN_EDGE_DATA_WITH_PTR 13
#define TAG_START_EDGES_DISTRIBUTION 14
#define TAG_START_EDGES_DISTRIBUTION_ASK 15
#define TAG_START_EDGES_DISTRIBUTION_ANSWER 16
#define TAG_PREPARE_COVERAGE_DISTRIBUTION_QUESTION 17
#define TAG_PREPARE_COVERAGE_DISTRIBUTION_ANSWER 18
#define TAG_PREPARE_COVERAGE_DISTRIBUTION 19
#define TAG_COVERAGE_DATA 20
#define TAG_COVERAGE_END 21
#define TAG_SEND_COVERAGE_VALUES 22
#define TAG_READY_TO_SEED 23
#define TAG_START_SEEDING 24
#define TAG_REQUEST_VERTEX_COVERAGE 25
#define TAG_REQUEST_VERTEX_COVERAGE_REPLY 26
#define TAG_REQUEST_VERTEX_KEY_AND_COVERAGE 28
#define TAG_REQUEST_VERTEX_KEY_AND_COVERAGE_REPLY 30
#define TAG_REQUEST_VERTEX_OUTGOING_EDGES 31
#define TAG_REQUEST_VERTEX_OUTGOING_EDGES_REPLY 32
#define TAG_SEEDING_IS_OVER 33
#define TAG_GOOD_JOB_SEE_YOU_SOON 34
#define TAG_I_GO_NOW 35
#define TAG_SET_WORD_SIZE 36
#define TAG_MASTER_IS_DONE_ATTACHING_READS 37
#define TAG_MASTER_IS_DONE_ATTACHING_READS_REPLY 38
#define TAG_FORWARD_TO_ATTACH_SEQUENCE_POINTER 39
#define TAG_FORWARD_TO_ATTACH_SEQUENCE_POINTER_REPLY 40
#define TAG_REQUEST_VERTEX_INGOING_EDGES 41
#define TAG_REQUEST_VERTEX_INGOING_EDGES_REPLY 42
#define TAG_EXTENSION_IS_DONE 43
#define TAG_ASK_EXTENSION 44
#define TAG_ASK_IS_ASSEMBLED 45
#define TAG_ASK_REVERSE_COMPLEMENT 46
#define TAG_REQUEST_VERTEX_POINTER 47
#define TAG_ASK_IS_ASSEMBLED_REPLY 48
#define TAG_MARK_AS_ASSEMBLED 49
#define TAG_ASK_EXTENSION_DATA 50
#define TAG_EXTENSION_DATA 51
#define TAG_EXTENSION_END 52
#define TAG_EXTENSION_DATA_END 53
#define TAG_ATTACH_SEQUENCE 54
#define TAG_REQUEST_READS 55
#define TAG_REQUEST_READS_REPLY 56
#define TAG_ASK_READ_VERTEX_AT_POSITION 57
#define TAG_ASK_READ_VERTEX_AT_POSITION_REPLY 58
#define TAG_ASK_READ_LENGTH 59
#define TAG_ASK_READ_LENGTH_REPLY 60
#define TAG_SAVE_WAVE_PROGRESSION 61
#define TAG_COPY_DIRECTIONS (sizeof(VERTEX_TYPE)*8-2)
#define TAG_ASSEMBLE_WAVES 63
#define TAG_COPY_DIRECTIONS_DONE 64
#define TAG_SAVE_WAVE_PROGRESSION_REVERSE 65
#define TAG_ASSEMBLE_WAVES_DONE 66
#define TAG_START_FUSION 67
#define TAG_FUSION_DONE 68
#define TAG_ASK_VERTEX_PATHS_SIZE 69
#define TAG_ASK_VERTEX_PATHS_SIZE_REPLY 70
#define TAG_GET_PATH_LENGTH 71
#define TAG_GET_PATH_LENGTH_REPLY 72
#define TAG_CALIBRATION_MESSAGE 73
#define TAG_BEGIN_CALIBRATION 74
#define TAG_END_CALIBRATION 75
#define TAG_COMMUNICATION_STABILITY_MESSAGE 76
#define TAG_ASK_VERTEX_PATH 77
#define TAG_ASK_VERTEX_PATH_REPLY 78
#define TAG_INDEX_PAIRED_SEQUENCE 79
#define TAG_HAS_PAIRED_READ 80
#define TAG_HAS_PAIRED_READ_REPLY 81
#define TAG_GET_PAIRED_READ 82
#define TAG_GET_PAIRED_READ_REPLY 83
#define TAG_CLEAR_DIRECTIONS 84
#define TAG_CLEAR_DIRECTIONS_REPLY 85
#define TAG_FINISH_FUSIONS 86
#define TAG_FINISH_FUSIONS_FINISHED 87
#define TAG_DISTRIBUTE_FUSIONS 88
#define TAG_DISTRIBUTE_FUSIONS_FINISHED 89
#define TAG_EXTENSION_START 90
#define TAG_ELIMINATE_PATH 91
#define TAG_GET_PATH_VERTEX 92
#define TAG_GET_PATH_VERTEX_REPLY 93
#define TAG_SET_COLOR_MODE 94

#define MASTER_RANK 0


// modes
#define MODE_EXTENSION_ASK 0
#define MODE_START_SEEDING 1
#define MODE_DO_NOTHING 2
#define MODE_ASK_EXTENSIONS 3
#define MODE_SEND_EXTENSION_DATA 4
#define MODE_ASSEMBLE_WAVES 5
#define MODE_COPY_DIRECTIONS 6
#define MODE_ASSEMBLE_GRAPH 7
#define MODE_FUSION 8
#define MODE_MASTER_ASK_CALIBRATION 9
#define MODE_PERFORM_CALIBRATION 10
#define MODE_FINISH_FUSIONS 11
#define MODE_DISTRIBUTE_FUSIONS 12
#define MODE_AMOS 13


// allocators size
// for MPI communications, memory is allocated and freed with OUTBOX_ALLOCATOR_CHUNK_SIZE and INBOX_ALLOCATOR_CHUNK_SIZE
// persistant memory are stored with PERSISTENT_ALLOCATOR_CHUNK_SIZE
#define SIZE_10MB 10*1024*1024
#define OUTBOX_ALLOCATOR_CHUNK_SIZE SIZE_10MB
#define DISTRIBUTION_ALLOCATOR_CHUNK_SIZE SIZE_10MB
#define INBOX_ALLOCATOR_CHUNK_SIZE SIZE_10MB
#define PERSISTENT_ALLOCATOR_CHUNK_SIZE SIZE_10MB

#define CALIBRATION_DURATION 10


#include<Machine.h>
#include<sstream>
#include<Message.h>
#include<time.h>
#include<BubbleTool.h>
#include<assert.h>
#include<common_functions.h>
#include<iostream>
#include<fstream>
#include<CoverageDistribution.h>
#include<string.h>
#include<SplayTreeIterator.h>
#include<mpi.h>
#include<Read.h>
#include<Loader.h>
#include<MyAllocator.h>
#include<unistd.h>

using namespace std;

void Machine::showUsage(){
	cout<<"Supported sequences file format: "<<endl;

	cout<<".fasta, .fastq, .sff"<<endl;

	cout<<endl;

	cout<<"Usage:"<<endl;
	cout<<endl;
	cout<<"Ray understands these commands:"<<endl;

    	cout<<" LoadSingleEndReads <sequencesFile> "<<endl;
    	cout<<"  aliases: -s, LoadSingleEndReads, -LoadSingleEndReads, --LoadSingleEndReads"<<endl;
	cout<<endl;
	cout<<" LoadPairedEndReads <leftSequencesFile> <rightSequencesFile> <fragmentLength> <standardDeviation> "<<endl;
	cout<<"  aliases: -p, LoadPairedEndReads, -LoadPairedEndReads, --LoadPairedEndReads"<<endl;
	cout<<endl;
	cout<<" OutputAmosFile "<<endl;
	cout<<"  aliases: -a, OutputAmosFile, -OutputAmosFile, --OutputAmosFile"<<endl;
    	cout<<endl;
	cout<<"Ray receives commands with command-line arguments or with a commands file. "<<endl;


	cout<<endl;
	cout<<"Outputs:"<<endl;
	cout<<" "<<m_parameters.getContigsFile()<<endl;
	cout<<" "<<m_parameters.getAmosFile()<<" (with OutputAmosFile)"<<endl;
	cout<<" "<<m_parameters.getCoverageDistributionFile()<<""<<endl;
	cout<<" "<<m_parameters.getParametersFile()<<endl;
	cout<<endl;
	cout<<"use --help to show this help"<<endl;
	cout<<endl;
}

/*
 * get the Directions taken by a vertex.
 *
 * m_Machine_getPaths_INITIALIZED must be set to false before any calls.
 * also, you must set m_Machine_getPaths_DONE to false;
 *
 * when done, m_Machine_getPaths_DONE is true
 * and
 * the result is in m_Machine_getPaths_result (a vector<Direction>)
 */
void Machine::getPaths(VERTEX_TYPE vertex){
	if(!m_Machine_getPaths_INITIALIZED){
		m_Machine_getPaths_INITIALIZED=true;
		m_FUSION_paths_requested=false;
		m_Machine_getPaths_DONE=false;
		m_Machine_getPaths_result.clear();
		return;
	}

	if(!m_FUSION_paths_requested){
		VERTEX_TYPE theVertex=vertex;
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
		message[0]=theVertex;
		Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(theVertex),TAG_ASK_VERTEX_PATHS_SIZE,getRank());
		m_outbox.push_back(aMessage);
		m_FUSION_paths_requested=true;
		m_FUSION_paths_received=false;
		m_FUSION_path_id=0;
		m_FUSION_path_requested=false;
		m_FUSION_receivedPaths.clear();
	}else if(m_FUSION_paths_received){
		if(m_FUSION_path_id<m_FUSION_numberOfPaths){
			if(!m_FUSION_path_requested){
				VERTEX_TYPE theVertex=vertex;
				VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
				message[0]=m_FUSION_path_id;
				Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(theVertex),TAG_ASK_VERTEX_PATH,getRank());
				m_outbox.push_back(aMessage);
				m_FUSION_path_requested=true;
				m_FUSION_path_received=false;
			}else if(m_FUSION_path_received){
				m_FUSION_path_id++;
				m_Machine_getPaths_result.push_back(m_FUSION_receivedPath);
				m_FUSION_path_requested=false;
			}
		}else{
			m_Machine_getPaths_DONE=true;
		}
	}
}

Machine::Machine(int argc,char**argv){
	m_argc=argc;
	m_argv=argv;
	m_bubbleData=new BubbleData();
	m_dfsData=new DepthFirstSearchData();
}

void Machine::start(){
	#ifdef SHOW_PROGRESS
	cout<<"sizeof(VERTEX_TYPE) "<<sizeof(VERTEX_TYPE)<<endl;
	cout<<"sizeof(Vertex) "<<sizeof(Vertex)<<endl;
	#endif
	time_t startingTime=time(NULL);
	m_lastTime=time(NULL);
	srand(m_lastTime);
	m_fusionStarted=false;
	m_COPY_ranks=-1;
	m_EXTENSION_numberOfRanksDone=0;
	m_colorSpaceMode=false;
	m_messageSentForEdgesDistribution=false;
	m_numberOfRanksDoneSeeding=0;
	m_calibrationAskedCalibration=false;
	m_calibrationIsDone=true; // set to false to perform a speed calibration.
	m_master_mode=MODE_DO_NOTHING;
	m_numberOfMachinesReadyForEdgesDistribution=-1;
	m_mode_EXTENSION=false;
	m_aborted=false;
	m_readyToSeed=0;
	m_wordSize=-1;
	m_reverseComplementVertex=false;
	m_last_value=0;
	m_speedLimitIsOn=false;// set to false to remove the speed limit everywhere.
	m_mode_send_ingoing_edges=false;
	m_mode_send_edge_sequence_id_position=0;
	m_mode_send_vertices=false;
	m_mode_sendDistribution=false;
	m_mode_send_outgoing_edges=false;
	m_mode_send_vertices_sequence_id=0;
	m_mode_send_vertices_sequence_id_position=0;
	m_reverseComplementEdge=false;
	m_calibration_MaxSpeed=99999999; // initial speed limit before calibration
	m_numberOfMachinesDoneSendingVertices=0;
	m_numberOfMachinesDoneSendingEdges=0;
	m_numberOfMachinesReadyToSendDistribution=0;
	m_numberOfMachinesDoneSendingCoverage=0;
	m_machineRank=0;
	m_messageSentForVerticesDistribution=false;
	m_sequence_ready_machines=0;
	m_isFinalFusion=false;

	m_outboxAllocator.constructor(OUTBOX_ALLOCATOR_CHUNK_SIZE);
	m_inboxAllocator.constructor(INBOX_ALLOCATOR_CHUNK_SIZE);
	m_distributionAllocator.constructor(DISTRIBUTION_ALLOCATOR_CHUNK_SIZE);
	m_persistentAllocator.constructor(PERSISTENT_ALLOCATOR_CHUNK_SIZE);
	m_directionsAllocator.constructor(PERSISTENT_ALLOCATOR_CHUNK_SIZE);


	m_mode=MODE_DO_NOTHING;
	m_mode_AttachSequences=false;
	m_startEdgeDistribution=false;

	m_ranksDoneAttachingReads=0;

	MPI_Init(&m_argc,&m_argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&m_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&m_size);



	if(isMaster()){
		#ifdef SHOW_PROGRESS
		cout<<"Rank "<<getRank()<<" welcomes you to the MPI_COMM_WORLD."<<endl;
		cout<<"Rank "<<getRank()<<": website -> http://denovoassembler.sf.net/"<<endl;
		#ifdef MPICH2_VERSION
		cout<<"Rank "<<getRank()<<": using MPICH2"<<endl;
		#else
			#ifdef OMPI_MPI_H
			cout<<"Rank "<<getRank()<<": using Open-MPI"<<endl;
			#else
			cout<<"Rank "<<getRank()<<": Warning, unknown implementation of MPI"<<endl;
			#endif
		#endif
		#else

		cout<<"Ray Copyright (C) 2010  Sébastien Boisvert, Jacques Corbeil, François Laviolette"<<endl;
    		cout<<"This program comes with ABSOLUTELY NO WARRANTY."<<endl;
    		cout<<"This is free software, and you are welcome to redistribute it"<<endl;
    		cout<<"under certain conditions; see \"gpl-3.0.txt\" for details."<<endl;
		cout<<endl;
 		cout<<"see http://denovoassembler.sf.net/"<<endl;
		cout<<endl;

		#endif
	}
	m_alive=true;
	m_welcomeStep=true;
	m_loadSequenceStep=false;
	m_vertices_sent=0;
	m_totalLetters=0;
	m_distribution_file_id=m_distribution_sequence_id=m_distribution_currentSequenceId=0;

	MPI_Barrier(MPI_COMM_WORLD);

	if(m_argc==1 or ((string)m_argv[1])=="--help"){
		if(isMaster()){
			showUsage();
		}
	}else{
		run();
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if(isMaster()){
		time_t endingTime=time(NULL);
		int difference=endingTime-startingTime;
		int minutes=difference/60;
		int seconds=difference%60;
		int hours=minutes/60;
		minutes=minutes%60;
		int days=hours/24;
		hours=hours%24;
		cout<<"\rElapsed time: "<<days<<" d "<<hours<<" h "<<minutes<<" min "<<seconds<<" s"<<endl;
	}
	MPI_Finalize();

}

/*
 * this is the function that runs a lots
 *
 * it
 * 	1) receives messages
 * 	2) free Request if any (only with MPICH2, Open-MPI is better designed and send small messages more efficiently!)
 * 	3) process message. The function that deals with a message is selected with the message's tag
 * 	4) process data, this depends on the master-mode and slave-mode states.
 * 	5) send messages
 */
void Machine::run(){
	#ifdef SHOW_PROGRESS
	if(isMaster()){
		cout<<"Rank "<<getRank()<<": I am the master among "<<getSize()<<" ranks in the MPI_COMM_WORLD."<<endl;
	}
	#endif

	if(isMaster()){
		cout<<"Starting "<<m_parameters.getEngineName()<<" "<<m_parameters.getVersion()<<endl;
		cout<<"Ray runs on "<<getSize()<<" MPI processes"<<endl;
	}
	while(isAlive()){
		receiveMessages(); 
		#ifdef MPICH2_VERSION
		checkRequests();
		#endif
		processMessages();
		processData();
		sendMessages();
	}
}

/*
 * free the memory of Requests
 */
void Machine::checkRequests(){
	MPI_Request theRequests[1024];
	MPI_Status theStatus[1024];
	
	for(int i=0;i<(int)m_pendingMpiRequest.size();i++){
		theRequests[i]=m_pendingMpiRequest[i];
	}
	MPI_Waitall(m_pendingMpiRequest.size(),theRequests,theStatus);
	m_pendingMpiRequest.clear();
}

/*
 * send messages,
 * if the message goes to self, do a memcpy!
 */
void Machine::sendMessages(){
	for(int i=0;i<(int)m_outbox.size();i++){

		Message*aMessage=&(m_outbox[i]);
		if(aMessage->getDestination()==getRank()){
			int sizeOfElements=8;
			if(aMessage->getTag()==TAG_SEND_SEQUENCE){
				sizeOfElements=1;
			}
			void*newBuffer=m_inboxAllocator.allocate(sizeOfElements*aMessage->getCount());
			memcpy(newBuffer,aMessage->getBuffer(),sizeOfElements*aMessage->getCount());
			aMessage->setBuffer(newBuffer);
			m_inbox.push_back(*aMessage);
			continue;
		}


		#ifdef MPICH2_VERSION // MPICH2 waits for the response on the other end.
		MPI_Request request;
		MPI_Status status;
		int flag;
		MPI_Isend(aMessage->getBuffer(), aMessage->getCount(), aMessage->getMPIDatatype(),aMessage->getDestination(),aMessage->getTag(), MPI_COMM_WORLD,&request);
		MPI_Test(&request,&flag,&status);
		if(!flag){
			m_pendingMpiRequest.push_back(request);
		}
		#else // Open-MPI-1.4.1 sends message eagerly, which is just a better design.
		MPI_Send(aMessage->getBuffer(), aMessage->getCount(), aMessage->getMPIDatatype(),aMessage->getDestination(),aMessage->getTag(), MPI_COMM_WORLD);
		#endif
	}

	m_outbox.clear();
	m_outboxAllocator.reset();
}

/*	
 * using Iprobe, probe for new messages as they arrive
 * if no more message are available, return.
 * messages are kept in the inbox.
 */

void Machine::receiveMessages(){
	int flag;
	MPI_Status status;
	MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
	int read=0;
	while(flag){
		read++;
		MPI_Datatype datatype=MPI_UNSIGNED_LONG_LONG;
		int sizeOfType=8;
		int tag=status.MPI_TAG;
		if(tag==TAG_SEND_SEQUENCE){
			datatype=MPI_BYTE;
			sizeOfType=1;
		}
		int source=status.MPI_SOURCE;
		int length;
		MPI_Get_count(&status,datatype,&length);
		void*incoming=(void*)m_inboxAllocator.allocate(length*sizeOfType);
		MPI_Status status2;
		MPI_Recv(incoming,length,datatype,source,tag,MPI_COMM_WORLD,&status2);
		Message aMessage(incoming,length,datatype,source,tag,source);
		m_inbox.push_back(aMessage);
		MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
	}

}

int Machine::getRank(){
	return m_rank;
}

/*
 * show progress on-screen.
 */
void Machine::showProgress(){
	printf("\r");
	int columns=10;
	int nn=m_lastTime%columns;
	
	for(int i=0;i<nn;i++){
		printf(".");
	}
	for(int i=0;i<columns-nn;i++){
		printf(" ");
	}
	fflush(stdout);

}

void Machine::loadSequences(){
	vector<string> allFiles=m_parameters.getAllFiles();
	if(m_distribution_reads.size()>0 and m_distribution_sequence_id>(int)m_distribution_reads.size()-1){
		// we reached the end of the file.
		m_distribution_file_id++;
		if(m_LOADER_isLeftFile){
			m_LOADER_numberOfSequencesInLeftFile=m_distribution_sequence_id;
		}
		m_distribution_sequence_id=0;
		m_distribution_reads.clear();
	}
	if(m_distribution_file_id>(int)allFiles.size()-1){
		m_loadSequenceStep=true;
		for(int i=0;i<getSize();i++){
			Message aMessage(NULL, 0, MPI_UNSIGNED_LONG_LONG, i, TAG_MASTER_IS_DONE_SENDING_ITS_SEQUENCES_TO_OTHERS,getRank());
			m_outbox.push_back(aMessage);
		}
		m_distributionAllocator.clear();
		m_distribution_reads.clear();
		return;
	}
	if(m_distribution_reads.size()==0){
		Loader loader;
		m_distribution_reads.clear();
		m_distributionAllocator.clear();
		m_distributionAllocator.constructor(DISTRIBUTION_ALLOCATOR_CHUNK_SIZE);
		#ifdef SHOW_PROGRESS
		cout<<"Rank "<<getRank()<<" loads "<<allFiles[m_distribution_file_id]<<"."<<endl;
		#else
		cout<<"\r"<<"Loading sequences ("<<allFiles[m_distribution_file_id]<<")"<<endl;
		#endif
		loader.load(allFiles[m_distribution_file_id],&m_distribution_reads,&m_distributionAllocator,&m_distributionAllocator);

		// write Reads in AMOS format.
		if(m_parameters.useAmos()){
			FILE*fp=m_bubbleData->m_amos;
			for(int i=0;i<(int)m_distribution_reads.size();i++){
				int iid=m_distribution_currentSequenceId+i;
				char*seq=m_distribution_reads.at(i)->getSeq();
				char*qlt=(char*)__Malloc(strlen(seq)+1);
				strcpy(qlt,seq);
				// spec: https://sourceforge.net/apps/mediawiki/amos/index.php?title=Message_Types#Sequence_t_:_Universal_t
				for(int j=0;j<(int)strlen(qlt);j++)
					qlt[j]='D';
				fprintf(fp,"{RED\niid:%i\neid:%i\nseq:\n%s\n.\nqlt:\n%s\n.\n}\n",iid+1,iid+1,seq,qlt);
				__Free(qlt);
			}
		}

		if(m_parameters.isLeftFile(m_distribution_file_id)){
			m_LOADER_isLeftFile=true;
		}else if(m_parameters.isRightFile(m_distribution_file_id)){
			m_LOADER_isRightFile=true;
			m_LOADER_averageFragmentLength=m_parameters.getFragmentLength(m_distribution_file_id);
			m_LOADER_deviation=m_parameters.getStandardDeviation(m_distribution_file_id);
		}else{
			m_LOADER_isLeftFile=m_LOADER_isRightFile=false;
		}

		#ifdef SHOW_PROGRESS
		cout<<"Rank "<<getRank()<<" has "<<m_distribution_reads.size()<<" sequences to distribute."<<endl;
		#else
		cout<<"Distributing sequences"<<endl;
		#endif
	}

	#ifndef SHOW_PROGRESS
	time_t tmp=time(NULL);
	if(tmp>m_lastTime){
		m_lastTime=tmp;
		showProgress();
	}
	#endif

	for(int i=0;i<1*getSize();i++){
		if(m_distribution_sequence_id>(int)m_distribution_reads.size()-1){
			#ifdef SHOW_PROGRESS
			cout<<"Rank "<<getRank()<<" distributes sequences, "<<m_distribution_reads.size()<<"/"<<m_distribution_reads.size()<<endl;
			#endif
			break;
		}
		int destination=m_distribution_currentSequenceId%getSize();

		if(destination<0 or destination>getSize()-1){
			cout<<destination<<" is bad"<<endl;
		}
		char*sequence=(m_distribution_reads)[m_distribution_sequence_id]->getSeq();
		#ifdef SHOW_PROGRESS
		if(m_distribution_sequence_id%1000000==0){
			cout<<"Rank "<<getRank()<<" distributes sequences, "<<m_distribution_sequence_id<<"/"<<m_distribution_reads.size()<<endl;
		}
		#endif
		Message aMessage(sequence, strlen(sequence), MPI_BYTE, destination, TAG_SEND_SEQUENCE,getRank());
		m_outbox.push_back(aMessage);

		// add paired information here..
		// algorithm follows.
		// check if current file is in a right file.
		// if yes, the leftDistributionCurrentSequenceId=m_distribution_currentSequenceId-NumberOfSequencesInRightFile.
		// the destination of a read i is i%getSize()
		// the readId on destination is i/getSize()
		// so, basically, send these bits to destination:
		//
		// rightSequenceGlobalId:= m_distribution_currentSequenceId
		// rightSequenceRank:= rightSequenceGlobalId%getSize
		// rightSequenceIdOnRank:= rightSequenceGlobalId/getSize
		// leftSequenceGlobalId:= rightSequenceGlobalId-numberOfSequencesInRightFile
		// leftSequenceRank:= leftSequenceGlobalId%getSize
		// leftSequenceIdOnRank:= leftSequenceGlobalId/getSize
		// averageFragmentLength:= ask the pairedFiles in m_parameters.
		if(m_LOADER_isRightFile){
			int rightSequenceGlobalId=m_distribution_currentSequenceId;
			int rightSequenceRank=rightSequenceGlobalId%getSize();
			int rightSequenceIdOnRank=rightSequenceGlobalId/getSize();
			int leftSequenceGlobalId=rightSequenceGlobalId-m_LOADER_numberOfSequencesInLeftFile;
			int leftSequenceRank=leftSequenceGlobalId%getSize();
			int leftSequenceIdOnRank=leftSequenceGlobalId/getSize();
			int averageFragmentLength=m_LOADER_averageFragmentLength;
			int deviation=m_LOADER_deviation;
			VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(5*sizeof(VERTEX_TYPE));
			message[0]=rightSequenceIdOnRank;
			message[1]=leftSequenceRank;
			message[2]=leftSequenceIdOnRank;
			message[3]=averageFragmentLength;
			message[4]=deviation;
			Message aMessage(message,5,MPI_UNSIGNED_LONG_LONG,rightSequenceRank,TAG_INDEX_PAIRED_SEQUENCE,getRank());
			m_outbox.push_back(aMessage);
		}

		m_distribution_currentSequenceId++;
		m_distribution_sequence_id++;
	}
}

void Machine::attachReads(){
	vector<string> allFiles=m_parameters.getAllFiles();
	if(m_distribution_reads.size()>0 and m_distribution_sequence_id>(int)m_distribution_reads.size()-1){
		m_distribution_file_id++;
		m_distribution_sequence_id=0;
		m_distribution_reads.clear();
	}
	if(m_distribution_file_id>(int)allFiles.size()-1){
		for(int i=0;i<getSize();i++){
			Message aMessage(NULL, 0, MPI_UNSIGNED_LONG_LONG, i, TAG_MASTER_IS_DONE_ATTACHING_READS,getRank());
			m_outbox.push_back(aMessage);
		}
		m_distribution_reads.clear();
		m_distributionAllocator.clear();
		m_mode_AttachSequences=false;
		return;
	}
	if(m_distribution_reads.size()==0){
		Loader loader;
		m_distribution_reads.clear();
		m_distributionAllocator.clear();
		m_distributionAllocator.constructor(DISTRIBUTION_ALLOCATOR_CHUNK_SIZE);
		#ifdef SHOW_PROGRESS
		cout<<"Rank "<<getRank()<<" loads "<<allFiles[m_distribution_file_id]<<"."<<endl;
		#else
		cout<<"\r"<<"Loading sequences ("<<allFiles[m_distribution_file_id]<<")"<<endl;
		#endif
		loader.load(allFiles[m_distribution_file_id],&m_distribution_reads,&m_distributionAllocator,&m_distributionAllocator);
		
		#ifdef SHOW_PROGRESS
		cout<<"Rank "<<getRank()<<" "<<m_distribution_reads.size()<<" sequences to attach"<<endl;
		#else
		cout<<"Indexing sequences"<<endl;
		#endif
	}
	#ifndef SHOW_PROGRESS
	time_t tmp=time(NULL);
	if(tmp>m_lastTime){
		m_lastTime=tmp;
		showProgress();
	}
	#endif

	for(int i=0;i<1;i++){
		if(m_distribution_sequence_id>(int)m_distribution_reads.size()-1){
			#ifdef SHOW_PROGRESS
			cout<<"Rank "<<getRank()<<" attaches sequences, "<<m_distribution_reads.size()<<"/"<<m_distribution_reads.size()<<endl;
			#endif
			break;
		}

		int destination=m_distribution_currentSequenceId%getSize();
		int sequenceIdOnDestination=m_distribution_currentSequenceId/getSize();

		if(destination<0 or destination>getSize()-1){
			cout<<destination<<" is bad"<<endl;
		}

		#ifdef SHOW_PROGRESS
		if(m_distribution_sequence_id%1000000==0){
			cout<<"Rank "<<getRank()<<" attaches sequences, "<<m_distribution_sequence_id<<"/"<<m_distribution_reads.size()<<endl;
		}
		#endif

		char*sequence=(m_distribution_reads)[m_distribution_sequence_id]->getSeq();
		if((int)strlen(sequence)<m_wordSize){
			m_distribution_currentSequenceId++;
			m_distribution_sequence_id++;
			return;
		}
		char vertexChar[100];
		memcpy(vertexChar,sequence,m_wordSize);
		vertexChar[m_wordSize]='\0';
		if(isValidDNA(vertexChar)){
			VERTEX_TYPE vertex=wordId(vertexChar);
			VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(4*sizeof(VERTEX_TYPE));
			message[0]=vertex;
			message[1]=destination;
			message[2]=sequenceIdOnDestination;
			message[3]='F';
			Message aMessage(message,4, MPI_UNSIGNED_LONG_LONG, vertexRank(vertex), TAG_ATTACH_SEQUENCE,getRank());
			m_outbox.push_back(aMessage);
		}
		memcpy(vertexChar,sequence+strlen(sequence)-m_wordSize,m_wordSize);
		vertexChar[m_wordSize]='\0';
		if(isValidDNA(vertexChar)){
			VERTEX_TYPE vertex=complementVertex(wordId(vertexChar),m_wordSize,m_colorSpaceMode);
			VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(4*sizeof(VERTEX_TYPE));
			message[0]=vertex;
			message[1]=destination;
			message[2]=sequenceIdOnDestination;
			message[3]='R';
			Message aMessage(message,4, MPI_UNSIGNED_LONG_LONG, vertexRank(vertex), TAG_ATTACH_SEQUENCE,getRank());
			m_outbox.push_back(aMessage);
		}

		m_distribution_currentSequenceId++;
		m_distribution_sequence_id++;
	}

}

void Machine::processMessage(Message*message){
	void*buffer=message->getBuffer();
	int count=message->getCount();
	int tag=message->getTag();
	int source=message->getSource();
	VERTEX_TYPE*incoming=(VERTEX_TYPE*)buffer;


	if(tag==TAG_VERTICES_DATA){
		int length=count;
		for(int i=0;i<length;i++){
			VERTEX_TYPE l=incoming[i];

			#ifdef SHOW_PROGRESS
			if(m_last_value!=(int)m_subgraph.size() and (int)m_subgraph.size()%100000==0){
				m_last_value=m_subgraph.size();
				cout<<"Rank "<<getRank()<<" has "<<m_subgraph.size()<<" vertices "<<endl;
			}
			#endif
			SplayNode<VERTEX_TYPE,Vertex>*tmp=m_subgraph.insert(l);
			#ifdef DEBUG
			assert(tmp!=NULL);
			#endif
			if(m_subgraph.inserted()){
				tmp->getValue()->constructor(); 
			}
			tmp->getValue()->setCoverage(tmp->getValue()->getCoverage()+1);
			#ifdef DEBUG
			assert(tmp->getValue()->getCoverage()>0);
			#endif
		}
	}else if(tag==TAG_EXTENSION_DATA_END){
		m_EXTENSION_currentRankIsDone=true;
	}else if(tag==TAG_EXTENSION_START){
		vector<VERTEX_TYPE> a;
		m_allPaths.push_back(a);
		int id=incoming[0];
		#ifdef DEBUG
		int rank=id%MAX_NUMBER_OF_MPI_PROCESSES;
		assert(rank<getSize());
		#endif
		m_identifiers.push_back(id);
	}else if(tag==TAG_EXTENSION_END){
	}else if(tag==TAG_START_FUSION){
		m_mode=MODE_FUSION;
		m_SEEDING_i=0;

		m_FUSION_direct_fusionDone=false;
		m_FUSION_first_done=false;
		m_FUSION_paths_requested=false;
	}else if(tag==TAG_BEGIN_CALIBRATION){
		m_calibration_numberOfMessagesSent=0;
		m_mode=MODE_PERFORM_CALIBRATION;
		#ifdef DEBUG

		#endif
	}else if(tag==TAG_END_CALIBRATION){
		m_mode=MODE_DO_NOTHING;
		m_calibration_MaxSpeed=m_calibration_numberOfMessagesSent/CALIBRATION_DURATION/getSize();
		cout<<"Rank "<<getRank()<<" MaximumSpeed (point-to-point)="<<m_calibration_MaxSpeed<<" messages/second"<<endl;
	}else if(tag==TAG_FINISH_FUSIONS){
		m_mode=MODE_FINISH_FUSIONS;
		m_FINISH_fusionOccured=false;
		m_SEEDING_i=0;
		m_EXTENSION_currentPosition=0;
		m_FUSION_first_done=false;
		m_Machine_getPaths_INITIALIZED=false;
		m_Machine_getPaths_DONE=false;
	}else if(tag==TAG_COPY_DIRECTIONS){
		m_mode=MODE_COPY_DIRECTIONS;
		SplayTreeIterator<VERTEX_TYPE,Vertex> seedingIterator(&m_subgraph);
		while(seedingIterator.hasNext()){
			SplayNode<VERTEX_TYPE,Vertex>*node=seedingIterator.next();
			m_SEEDING_nodes.push_back(node->getKey());
		}
		m_SEEDING_i=0;
	}else if(tag==TAG_EXTENSION_IS_DONE){
		m_EXTENSION_numberOfRanksDone++;
		m_EXTENSION_currentRankIsDone=true;
	}else if(tag==TAG_SET_WORD_SIZE){
		m_wordSize=incoming[0];
	}else if(tag==TAG_SET_COLOR_MODE){
		m_colorSpaceMode=incoming[0];
	}else if(tag==TAG_START_SEEDING){
		m_mode=MODE_START_SEEDING;
		map<int,map<int,int> > edgesDistribution;

		SplayTreeIterator<VERTEX_TYPE,Vertex> seedingIterator(&m_subgraph);
		while(seedingIterator.hasNext()){
			SplayNode<VERTEX_TYPE,Vertex>*node=seedingIterator.next();
			edgesDistribution[node->getValue()->getIngoingEdges(node->getKey(),m_wordSize).size()][node->getValue()->getOutgoingEdges(node->getKey(),m_wordSize).size()]++;
			m_SEEDING_nodes.push_back(node->getKey());
		}
		#ifdef DEBUG
		//cout<<"Ingoing and outgoing edges."<<endl;
		for(map<int,map<int,int> >::iterator i=edgesDistribution.begin();i!=edgesDistribution.end();++i){
			for(map<int,int>::iterator j=i->second.begin();j!=i->second.end();++j){
				//cout<<i->first<<" "<<j->first<<" "<<j->second<<endl;
			}
		}
		#endif
		m_SEEDING_NodeInitiated=false;
		m_SEEDING_i=0;
	}else if(tag==TAG_FINISH_FUSIONS_FINISHED){
		m_FINISH_n++;
		if(incoming[0]){
			m_nextReductionOccured=incoming[0];
		}
	}else if(tag==TAG_CLEAR_DIRECTIONS){
	
		// clearing old data too!.
		m_FINISH_pathLengths.clear();

		// clear graph
		SplayTreeIterator<VERTEX_TYPE,Vertex> iterator(&m_subgraph);
		while(iterator.hasNext()){
			iterator.next()->getValue()->clearDirections();
		}
		m_directionsAllocator.clear();
		m_directionsAllocator.constructor(PERSISTENT_ALLOCATOR_CHUNK_SIZE);


		// add the FINISHING bits
		for(int i=0;i<(int)m_FINISH_newFusions.size();i++){
			#ifdef SHOW_PROGRESS
			cout<<"Adding "<<m_FINISH_newFusions[i].size()<<endl;
			#endif
			m_EXTENSION_contigs.push_back(m_FINISH_newFusions[i]);
		}


		#ifdef DEBUG
		//cout<<"Rank "<<getRank()<<" has "<<m_EXTENSION_contigs.size()<<" hyperfusions, new="<<m_FINISH_newFusions.size()<<" deleted="<<m_FUSION_eliminated.size()<<endl;
		#endif

		m_FINISH_newFusions.clear();

		vector<vector<VERTEX_TYPE> > fusions;
		for(int i=0;i<(int)m_EXTENSION_contigs.size();i++){
			int id=m_EXTENSION_identifiers[i];
			if(m_FUSION_eliminated.count(id)==0){
				fusions.push_back(m_EXTENSION_contigs[i]);
				vector<VERTEX_TYPE> rc;
				for(int j=m_EXTENSION_contigs[i].size()-1;j>=0;j--){
					rc.push_back(complementVertex(m_EXTENSION_contigs[i][j],m_wordSize,m_colorSpaceMode));
				}
				fusions.push_back(rc);
			}
		}

		m_EXTENSION_identifiers.clear();
		m_FUSION_eliminated.clear();
		for(int i=0;i<(int)fusions.size();i++){
			int id=i*MAX_NUMBER_OF_MPI_PROCESSES+getRank();
			#ifdef DEBUG
			assert(id%MAX_NUMBER_OF_MPI_PROCESSES<getSize());
			#endif
			m_EXTENSION_identifiers.push_back(id);
		}

		for(int i=0;i<(int)m_EXTENSION_identifiers.size();i++){
			int id=m_EXTENSION_identifiers[i];
			m_FUSION_identifier_map[id]=i;
		}

		m_EXTENSION_contigs.clear();
		m_EXTENSION_contigs=fusions;

		Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,source,TAG_CLEAR_DIRECTIONS_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_CLEAR_DIRECTIONS_REPLY){
		m_CLEAR_n++;
	}else if(tag==TAG_INDEX_PAIRED_SEQUENCE){
		PairedRead*t=(PairedRead*)m_persistentAllocator.allocate(sizeof(PairedRead));
		t->constructor(incoming[1],incoming[2],incoming[3],incoming[4]);
		m_myReads[incoming[0]]->setPairedRead(t);
	}else if(tag==TAG_COMMUNICATION_STABILITY_MESSAGE){
	}else if(tag==TAG_GET_PATH_VERTEX){
		int id=incoming[0];
		int position=incoming[1];
		#ifdef DEBUG
		assert(m_FUSION_identifier_map.count(id)>0);
		#endif
		#ifdef DEBUG
		if(position>=(int)m_EXTENSION_contigs[m_FUSION_identifier_map[id]].size()){
			cout<<"Pos="<<position<<" Length="<<m_EXTENSION_contigs[m_FUSION_identifier_map[id]].size()<<endl;
		}
		assert(position<(int)m_EXTENSION_contigs[m_FUSION_identifier_map[id]].size());
		#endif
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(sizeof(VERTEX_TYPE));
		message[0]=m_EXTENSION_contigs[m_FUSION_identifier_map[id]][position];
		Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,source,TAG_GET_PATH_VERTEX_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_GET_PATH_VERTEX_REPLY){
		m_FINISH_vertex_received=true;
		m_FINISH_received_vertex=incoming[0];
	}else if(tag==TAG_GET_PATH_LENGTH){
		int id=incoming[0];
		int length=0;
		#ifdef DEBUG
		assert(m_FUSION_identifier_map.count(id)>0);
		#endif
		if(m_FUSION_identifier_map.count(id)>0){
			length=m_EXTENSION_contigs[m_FUSION_identifier_map[id]].size();
		}

		#ifdef DEBUG
		assert(length>0);
		#endif
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(sizeof(VERTEX_TYPE));
		message[0]=length;
		Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,source,TAG_GET_PATH_LENGTH_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_GET_PATH_LENGTH_REPLY){
		m_FUSION_receivedLength=incoming[0];
		m_FUSION_pathLengthReceived=true;
	}else if(tag==TAG_REQUEST_VERTEX_COVERAGE){
		SplayNode<VERTEX_TYPE,Vertex>*node=m_subgraph.find(incoming[0]);
		#ifdef DEBUG
		assert(node!=NULL);
		#endif
		VERTEX_TYPE coverage=node->getValue()->getCoverage();
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
		message[0]=coverage;
		Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,source,TAG_REQUEST_VERTEX_COVERAGE_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_DISTRIBUTE_FUSIONS){
		m_mode=MODE_DISTRIBUTE_FUSIONS;
		m_SEEDING_i=0;
		m_EXTENSION_currentPosition=0;
	}else if(tag==TAG_DISTRIBUTE_FUSIONS_FINISHED){
		m_DISTRIBUTE_n++;
	}else if(tag==TAG_HAS_PAIRED_READ){
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
		message[0]=m_myReads[incoming[0]]->hasPairedRead();
		Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,source,TAG_HAS_PAIRED_READ_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_HAS_PAIRED_READ_REPLY){
		m_EXTENSION_hasPairedReadAnswer=incoming[0];
		m_EXTENSION_hasPairedReadReceived=true;
	}else if(tag==TAG_REQUEST_VERTEX_INGOING_EDGES){
		SplayNode<VERTEX_TYPE,Vertex>*node=m_subgraph.find(incoming[0]);
		#ifdef DEBUG
		assert(node!=NULL);
		#endif
		vector<VERTEX_TYPE> ingoingEdges=node->getValue()->getIngoingEdges(incoming[0],m_wordSize);
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(ingoingEdges.size()*sizeof(VERTEX_TYPE));
		for(int i=0;i<(int)ingoingEdges.size();i++){
			message[i]=ingoingEdges[i];
		}
		Message aMessage(message,ingoingEdges.size(),MPI_UNSIGNED_LONG_LONG,source,TAG_REQUEST_VERTEX_INGOING_EDGES_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_CALIBRATION_MESSAGE){
	}else if(tag==TAG_ASK_VERTEX_PATHS_SIZE){
		#ifdef DEBUG
		if(m_subgraph.find(incoming[0])==NULL){
			cout<<"Vertex "<<incoming[0]<<" is not here."<<endl;
		}
		assert(m_subgraph.find(incoming[0])!=NULL);
		#endif
		vector<Direction> paths=m_subgraph.find(incoming[0])->getValue()->getDirections();
		m_FUSION_cachedDirections[source]=paths;
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
		message[0]=paths.size();
		Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,source,TAG_ASK_VERTEX_PATHS_SIZE_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_ASK_VERTEX_PATHS_SIZE_REPLY){
		m_FUSION_paths_received=true;
		m_FUSION_receivedPaths.clear();
		m_FUSION_numberOfPaths=incoming[0];
	}else if(tag==TAG_ELIMINATE_PATH){
		m_FUSION_eliminated.insert(incoming[0]);
	}else if(tag==TAG_ASK_VERTEX_PATH){
		int i=incoming[0];
		Direction d=m_FUSION_cachedDirections[source][i];
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(2*sizeof(VERTEX_TYPE));
		message[0]=d.getWave();
		message[1]=d.getProgression();
		Message aMessage(message,2,MPI_UNSIGNED_LONG_LONG,source,TAG_ASK_VERTEX_PATH_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_ASK_VERTEX_PATH_REPLY){
		m_FUSION_path_received=true;
		int pathId=incoming[0];
		int position=incoming[1];
		m_FUSION_receivedPath.constructor(pathId,position);
	}else if(tag==TAG_ASK_EXTENSION_DATA){
		m_mode=MODE_SEND_EXTENSION_DATA;
		m_SEEDING_i=0;
		m_EXTENSION_currentPosition=0;
	}else if(tag==TAG_GET_PAIRED_READ){
		PairedRead*t=m_myReads[incoming[0]]->getPairedRead();
		if(t==NULL){
			cout<<"Fatal, no paired read."<<endl;
		}
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(4*sizeof(VERTEX_TYPE));
		message[0]=t->getRank();
		message[1]=t->getId();
		message[2]=t->getAverageFragmentLength();
		message[3]=t->getStandardDeviation();
		Message aMessage(message,4,MPI_UNSIGNED_LONG_LONG,source,TAG_GET_PAIRED_READ_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_GET_PAIRED_READ_REPLY){
		m_EXTENSION_pairedRead.constructor(incoming[0],incoming[1],incoming[2],incoming[3]);
		m_EXTENSION_pairedSequenceReceived=true;
	}else if(tag==TAG_ASSEMBLE_WAVES){
		m_mode=MODE_ASSEMBLE_WAVES;
		m_SEEDING_i=0;
	}else if(tag==TAG_COPY_DIRECTIONS_DONE){
		m_COPY_ranks++;
	}else if(tag==TAG_SAVE_WAVE_PROGRESSION){
		SplayNode<VERTEX_TYPE,Vertex>*node=m_subgraph.find(incoming[0]);
		int wave=incoming[1];
		int progression=incoming[2];
		node->getValue()->addDirection(wave,progression,&m_directionsAllocator);
	}else if(tag==TAG_SAVE_WAVE_PROGRESSION_REVERSE){
	}else if(tag==TAG_EXTENSION_DATA){
		m_allPaths[m_allPaths.size()-1].push_back(incoming[0]);
	}else if(tag==TAG_FUSION_DONE){
		m_FUSION_numberOfRanksDone++;
	}else if(tag==TAG_ASK_EXTENSION){
		m_EXTENSION_initiated=false;
		m_mode_EXTENSION=true;
		m_last_value=-1;
	}else if(tag==TAG_ASK_REVERSE_COMPLEMENT){
		SplayNode<VERTEX_TYPE,Vertex>*node=(SplayNode<VERTEX_TYPE,Vertex>*)incoming[0];
		VERTEX_TYPE value=node->getKey();
		VERTEX_TYPE reverseComplement=complementVertex(value,m_wordSize,m_colorSpaceMode);
		int rank=vertexRank(reverseComplement);
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(2*sizeof(VERTEX_TYPE));
		Message aMessage(message,2,MPI_UNSIGNED_LONG_LONG,rank,TAG_REQUEST_VERTEX_POINTER,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_MARK_AS_ASSEMBLED){
	}else if(tag==TAG_ASK_IS_ASSEMBLED){
		SplayNode<VERTEX_TYPE,Vertex>*node=m_subgraph.find(incoming[0]);
		bool isAssembled=node->getValue()->isAssembled();
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
		message[0]=isAssembled;
		Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,source,TAG_ASK_IS_ASSEMBLED_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_ASK_IS_ASSEMBLED_REPLY){
		m_EXTENSION_VertexAssembled_received=true;
		m_EXTENSION_vertexIsAssembledResult=(bool)incoming[0];
	}else if(tag==TAG_REQUEST_VERTEX_OUTGOING_EDGES){
		vector<VERTEX_TYPE> outgoingEdges=m_subgraph.find(incoming[0])->getValue()->getOutgoingEdges(incoming[0],m_wordSize);
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(outgoingEdges.size()*sizeof(VERTEX_TYPE));
		for(int i=0;i<(int)outgoingEdges.size();i++){
			message[i]=outgoingEdges[i];
		}
		Message aMessage(message,outgoingEdges.size(),MPI_UNSIGNED_LONG_LONG,source,TAG_REQUEST_VERTEX_OUTGOING_EDGES_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_GOOD_JOB_SEE_YOU_SOON){
		m_alive=false;
	}else if(tag==TAG_SEEDING_IS_OVER){
		m_numberOfRanksDoneSeeding++;
	}else if(tag==TAG_ATTACH_SEQUENCE){
		VERTEX_TYPE vertex=incoming[0];
		int rank=incoming[1];
		int sequenceIdOnDestination=(int)incoming[2];
		char strand=(char)incoming[3];
		#ifdef DEBUG
		if(m_subgraph.find(vertex)==NULL){
			cout<<"Strand="<<strand<<endl;
		}
		assert(m_subgraph.find(vertex)!=NULL);
		#endif
		m_subgraph.find(vertex)->getValue()->addRead(rank,sequenceIdOnDestination,strand,&m_persistentAllocator);
	}else if(tag==TAG_REQUEST_VERTEX_INGOING_EDGES_REPLY){
		m_SEEDING_receivedIngoingEdges.clear();
		for(int i=0;i<count;i++){
			m_SEEDING_receivedIngoingEdges.push_back(incoming[i]);
		}
		m_SEEDING_InedgesReceived=true;
	}else if(tag==TAG_REQUEST_VERTEX_OUTGOING_EDGES_REPLY){
		m_SEEDING_receivedOutgoingEdges.clear();
		for(int i=0;i<count;i++){
			m_SEEDING_receivedOutgoingEdges.push_back(incoming[i]);
		}
		m_SEEDING_edgesReceived=true;
	}else if(tag==TAG_ASSEMBLE_WAVES_DONE){
		m_EXTENSION_currentRankIsDone=true;
	}else if(tag==TAG_MASTER_IS_DONE_ATTACHING_READS){
		Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,source,TAG_MASTER_IS_DONE_ATTACHING_READS_REPLY,getRank());
		m_outbox.push_back(aMessage);
		#ifdef SHOW_PROGRESS
		cout<<"Rank "<<getRank()<<" attaches sequences. "<<m_myReads.size()<<"/"<<m_myReads.size()<<" (DONE)"<<endl;
		#endif
	}else if(tag==TAG_MASTER_IS_DONE_ATTACHING_READS_REPLY){
		m_ranksDoneAttachingReads++;
	}else if(tag==TAG_REQUEST_VERTEX_KEY_AND_COVERAGE){
		SplayNode<VERTEX_TYPE,Vertex>*node=(SplayNode<VERTEX_TYPE,Vertex>*)incoming[0];
		VERTEX_TYPE key=node->getKey();
		VERTEX_TYPE coverage=node->getValue()->getCoverage();
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(2*sizeof(VERTEX_TYPE));
		message[0]=key;
		message[1]=coverage;
		Message aMessage(message,2,MPI_UNSIGNED_LONG_LONG,source,TAG_REQUEST_VERTEX_KEY_AND_COVERAGE_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_REQUEST_VERTEX_KEY_AND_COVERAGE_REPLY){
		m_SEEDING_receivedKey=incoming[0];
		m_SEEDING_receivedVertexCoverage=incoming[1];
		m_SEEDING_vertexKeyAndCoverageReceived=true;
	}else if(tag==TAG_REQUEST_VERTEX_COVERAGE_REPLY){
		m_SEEDING_receivedVertexCoverage=incoming[0];
		m_SEEDING_vertexCoverageReceived=true;
	}else if(tag==TAG_COVERAGE_DATA){
		int length=count;

		for(int i=0;i<length;i+=2){
			int coverage=incoming[i+0];
			VERTEX_TYPE count=incoming[i+1];
			m_coverageDistribution[coverage]+=count;
		}
	}else if(tag==TAG_SEND_COVERAGE_VALUES){
		m_minimumCoverage=incoming[0];
		m_seedCoverage=incoming[1];
		m_peakCoverage=incoming[2];
		#ifdef SHOW_PROGRESS
		cout<<"Min="<<m_minimumCoverage<<" Seed="<<m_seedCoverage<<" Peak="<<m_peakCoverage<<endl;
		#endif
		Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,MASTER_RANK,TAG_READY_TO_SEED,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_READY_TO_SEED){
		m_readyToSeed++;
	}else if(tag==TAG_OUT_EDGES_DATA){
		int length=count;

		for(int i=0;i<(int)length;i+=2){
			VERTEX_TYPE prefix=incoming[i+0];
			VERTEX_TYPE suffix=incoming[i+1];
			#ifdef DEBUG
			assert(m_subgraph.find(prefix)!=NULL);
			#endif
			m_subgraph.find(prefix)->getValue()->addOutgoingEdge(suffix,m_wordSize,&m_persistentAllocator);
			#ifdef DEBUG
			vector<VERTEX_TYPE> newEdges=m_subgraph.find(prefix)->getValue()->getOutgoingEdges(prefix,m_wordSize);
			bool found=false;
			for(int i=0;i<(int)newEdges.size();i++){
				if(newEdges[i]==suffix)
					found=true;
			}
			if(newEdges.size()==0){
				cout<<"prefix,suffix"<<endl;
				coutBIN(prefix);
				coutBIN(suffix);
				cout<<idToWord(prefix,m_wordSize)<<endl;
				cout<<idToWord(suffix,m_wordSize)<<endl;
			}
			assert(newEdges.size()>0);
			if(!found){
				cout<<"prefix,suffix"<<endl;
				coutBIN(prefix);
				coutBIN(suffix);
				cout<<idToWord(prefix,m_wordSize)<<endl;
				cout<<idToWord(suffix,m_wordSize)<<endl;

				cout<<" ARC"<<endl;
				coutBIN(prefix);
				coutBIN(suffix);
				cout<<"EDges."<<endl;
				for(int i=0;i<(int)newEdges.size();i++){
					coutBIN(newEdges[i]);
				}

			}
			assert(found);
			#endif
		}
	}else if(tag==TAG_ASK_READ_LENGTH){
		int length=m_myReads[incoming[0]]->length();
		
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
		message[0]=length;
		Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,source,TAG_ASK_READ_LENGTH_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_ASK_READ_LENGTH_REPLY){
		m_EXTENSION_readLength_received=true;
		m_EXTENSION_receivedLength=incoming[0];
	}else if(tag==TAG_ASK_READ_VERTEX_AT_POSITION){
		char strand=incoming[2];
		VERTEX_TYPE vertex=m_myReads[incoming[0]]->Vertex(incoming[1],m_wordSize,strand,m_colorSpaceMode);
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
		message[0]=vertex;
		Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,source,TAG_ASK_READ_VERTEX_AT_POSITION_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_ASK_READ_VERTEX_AT_POSITION_REPLY){
		m_EXTENSION_read_vertex_received=true;
		m_EXTENSION_receivedReadVertex=((VERTEX_TYPE*)buffer)[0];
	}else if(tag==TAG_IN_EDGES_DATA){
		int length=count;

		for(int i=0;i<(int)length;i+=2){
			VERTEX_TYPE prefix=incoming[i+0];
			VERTEX_TYPE suffix=incoming[i+1];
			m_subgraph.find(suffix)->getValue()->addIngoingEdge(prefix,m_wordSize,&m_persistentAllocator);
			#ifdef DEBUG
			bool found=false;
			vector<VERTEX_TYPE> edges=m_subgraph.find(suffix)->getValue()->getIngoingEdges(suffix,m_wordSize);
			for(int i=0;i<(int)edges.size();i++){
				if(edges[i]==prefix)
					found=true;
			}
			assert(found);
			#endif
		}
	}else if(tag==TAG_WELCOME){

	}else if(tag==TAG_SEND_SEQUENCE){
		int length=count;
		char*incoming=(char*)m_inboxAllocator.allocate(count*sizeof(char)+1);
		for(int i=0;i<(int)length;i++)
			incoming[i]=((char*)buffer)[i];

		incoming[length]='\0';
		Read*myRead=(Read*)m_persistentAllocator.allocate(sizeof(Read));
		myRead->copy(NULL,incoming,&m_persistentAllocator);
		m_myReads.push_back(myRead);
		#ifdef SHOW_PROGRESS
		if(m_myReads.size()%100000==0){
			cout<<"Rank "<<getRank()<<" has "<<m_myReads.size()<<" sequences"<<endl;
		}
		#endif
	}else if(tag==TAG_MASTER_IS_DONE_SENDING_ITS_SEQUENCES_TO_OTHERS){
		#ifdef SHOW_PROGRESS
		cout<<"Rank "<<getRank()<<" has "<<m_myReads.size()<<" sequences"<<endl;
		#endif
		Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,source,TAG_SEQUENCES_READY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_SHOW_VERTICES){
		#ifdef SHOW_PROGRESS
		cout<<"Rank "<<getRank()<<" has "<<m_subgraph.size()<<" vertices (DONE)"<<endl;
		#endif
	}else if(tag==TAG_SEQUENCES_READY){
		m_sequence_ready_machines++;
	}else if(tag==TAG_START_EDGES_DISTRIBUTION_ANSWER){
		m_numberOfMachinesReadyForEdgesDistribution++;
	}else if(tag==TAG_PREPARE_COVERAGE_DISTRIBUTION_ANSWER){
		m_numberOfMachinesReadyToSendDistribution++;
	}else if(tag==TAG_PREPARE_COVERAGE_DISTRIBUTION){
		m_mode_send_coverage_iterator=0;
		m_mode_sendDistribution=true;
	}else if(tag==TAG_START_EDGES_DISTRIBUTION_ASK){
		Message aMessage(NULL, 0, MPI_UNSIGNED_LONG_LONG, source, TAG_START_EDGES_DISTRIBUTION_ANSWER,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_PREPARE_COVERAGE_DISTRIBUTION_QUESTION){
		Message aMessage(NULL, 0, MPI_UNSIGNED_LONG_LONG, source, TAG_PREPARE_COVERAGE_DISTRIBUTION_ANSWER,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_START_EDGES_DISTRIBUTION){
		m_mode_send_outgoing_edges=true;
		m_mode_send_edge_sequence_id=0;
	}else if(tag==TAG_START_VERTICES_DISTRIBUTION){
		#ifdef SHOW_PROGRESS
		cout<<"TAG_START_VERTICES_DISTRIBUTION"<<endl;
		#endif
		m_mode_send_vertices=true;
		m_mode_send_vertices_sequence_id=0;
	}else if(tag==TAG_VERTICES_DISTRIBUTED){
		m_numberOfMachinesDoneSendingVertices++;
	}else if(tag==TAG_COVERAGE_END){
		m_numberOfMachinesDoneSendingCoverage++;
	}else if(tag==TAG_REQUEST_READS){
		vector<ReadAnnotation> reads;
		ReadAnnotation*e=m_subgraph.find(incoming[0])->getValue()->getReads();
		while(e!=NULL){
			reads.push_back(*e);
			e=e->getNext();
		}
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(3*sizeof(VERTEX_TYPE)*reads.size());
		int j=0;
		for(int i=0;i<(int)reads.size();i++){
			message[j++]=reads[i].getRank();
			message[j++]=reads[i].getReadIndex();
			message[j++]=reads[i].getStrand();
		}
		Message aMessage(message,3*reads.size(),MPI_UNSIGNED_LONG_LONG,source,TAG_REQUEST_READS_REPLY,getRank());
		m_outbox.push_back(aMessage);
	}else if(tag==TAG_REQUEST_READS_REPLY){
		m_EXTENSION_receivedReads.clear();
		for(int i=0;i<count;i+=3){
			int rank=incoming[i];
			int index=incoming[i+1];
			char strand=incoming[i+2];
			ReadAnnotation e;
			e.constructor(rank,index,strand);
			m_EXTENSION_receivedReads.push_back(e);
		}
		m_EXTENSION_reads_received=true;
	}else if(tag==TAG_EDGES_DISTRIBUTED){
		m_numberOfMachinesDoneSendingEdges++;
	}else{
		cout<<"Unknown tag"<<tag<<endl;
	}
}

/*
 * finish hyper fusions now!
 */
void Machine::finishFusions(){
	// finishing is broken?
	if(m_SEEDING_i==(int)m_EXTENSION_contigs.size()){
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
		message[0]=m_FINISH_fusionOccured;
		Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,MASTER_RANK,TAG_FINISH_FUSIONS_FINISHED,getRank());
		m_outbox.push_back(aMessage);
		m_mode=MODE_DO_NOTHING;
		return;
	}
	int overlapMinimumLength=1000;
	if((int)m_EXTENSION_contigs[m_SEEDING_i].size()<overlapMinimumLength){
		#ifdef SHOW_PROGRESS
		cout<<"No overlap possible m_SEEDING_i="<<m_SEEDING_i<<" size="<<m_EXTENSION_contigs[m_SEEDING_i].size()<<endl;
		#endif
		m_SEEDING_i++;
		m_FINISH_vertex_requested=false;
		m_EXTENSION_currentPosition=0;
		m_FUSION_pathLengthRequested=false;
		m_Machine_getPaths_INITIALIZED=false;
		m_Machine_getPaths_DONE=false;
		m_checkedValidity=false;
		return;
	}
	// check if the path begins with someone else.
	
	int currentId=m_EXTENSION_identifiers[m_SEEDING_i];
	// don't do it if it is removed.

	// start threading the extension
	// as the algorithm advance on it, it stores the path positions.
	// when it reaches a choice, it will use the available path as basis.
	
	// we have the extension in m_EXTENSION_contigs[m_SEEDING_i]
	// we get the paths with getPaths
	bool done=false;
	if(m_EXTENSION_currentPosition<(int)m_EXTENSION_contigs[m_SEEDING_i].size()){
		if(!m_Machine_getPaths_DONE){
			getPaths(m_EXTENSION_contigs[m_SEEDING_i][m_EXTENSION_currentPosition]);
		}else{
			vector<Direction> a;
			for(int i=0;i<(int)m_Machine_getPaths_result.size();i++){
				if(m_Machine_getPaths_result[i].getWave()!=currentId){
					a.push_back(m_Machine_getPaths_result[i]);
				}
			}
			m_FINISH_pathsForPosition.push_back(a);
			if(m_EXTENSION_currentPosition==0){
				vector<VERTEX_TYPE> a;
				m_FINISH_newFusions.push_back(a);
				m_FINISH_vertex_requested=false;
				m_FUSION_eliminated.insert(currentId);
				m_FUSION_pathLengthRequested=false;
				m_checkedValidity=false;
			}
			VERTEX_TYPE vertex=m_EXTENSION_contigs[m_SEEDING_i][m_EXTENSION_currentPosition];
			m_FINISH_newFusions[m_FINISH_newFusions.size()-1].push_back(vertex);
			m_EXTENSION_currentPosition++;
			m_Machine_getPaths_DONE=false;
			m_Machine_getPaths_INITIALIZED=false;
		}
	}else if(!m_checkedValidity){
		done=true;
		vector<Direction> directions1=m_FINISH_pathsForPosition[m_FINISH_pathsForPosition.size()-1];
		vector<Direction> directions2=m_FINISH_pathsForPosition[m_FINISH_pathsForPosition.size()-overlapMinimumLength];
		int hits=0;
		for(int i=0;i<(int)directions1.size();i++){
			for(int j=0;j<(int)directions2.size();j++){
				if(directions1[i].getWave()==directions2[j].getWave()){
					int progression=directions1[i].getProgression();
					int otherProgression=directions2[j].getProgression();
					if(progression-otherProgression+1==overlapMinimumLength){
						// this is 
						done=false;
						hits++;
						m_selectedPath=directions1[i].getWave();
						m_selectedPosition=directions1[i].getProgression();
					}
				}
			}
		}
		if(hits>1){// we don't support that right now.
			done=true;
		}
		m_checkedValidity=true;
	}else{
		// check if it is there for at least overlapMinimumLength
		int pathId=m_selectedPath;
		int progression=m_selectedPosition;

		// only one path, just go where it goes...
		// except if it has the same number of vertices and
		// the same start and end.
		if(m_FINISH_pathLengths.count(pathId)==0){
			if(!m_FUSION_pathLengthRequested){
				int rankId=pathId%MAX_NUMBER_OF_MPI_PROCESSES;
				VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(sizeof(VERTEX_TYPE));
				message[0]=pathId;
				Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,rankId,TAG_GET_PATH_LENGTH,getRank());
				m_outbox.push_back(aMessage);
				m_FUSION_pathLengthRequested=true;
				m_FUSION_pathLengthReceived=false;
			}else if(m_FUSION_pathLengthReceived){
				m_FINISH_pathLengths[pathId]=m_FUSION_receivedLength;
			}
		}else if(m_FINISH_pathLengths[pathId]!=(int)m_EXTENSION_contigs[m_SEEDING_i].size()){// avoid fusion of same length.
			int nextPosition=progression+1;
			if(nextPosition<m_FINISH_pathLengths[pathId]){
				// get the vertex
				// get its paths,
				// and continue...
				if(!m_FINISH_vertex_requested){
					int rankId=pathId%MAX_NUMBER_OF_MPI_PROCESSES;
					VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(sizeof(VERTEX_TYPE)*2);
					message[0]=pathId;
					message[1]=nextPosition;
					Message aMessage(message,2,MPI_UNSIGNED_LONG_LONG,rankId,TAG_GET_PATH_VERTEX,getRank());
					m_outbox.push_back(aMessage);
					m_FINISH_vertex_requested=true;
					m_FINISH_vertex_received=false;
				}else if(m_FINISH_vertex_received){
					if(!m_Machine_getPaths_DONE){
						getPaths(m_FINISH_received_vertex);
					}else{
						m_FINISH_pathsForPosition.push_back(m_Machine_getPaths_result);
						m_FINISH_newFusions[m_FINISH_newFusions.size()-1].push_back(m_FINISH_received_vertex);
						m_FINISH_vertex_requested=false;
						m_Machine_getPaths_INITIALIZED=false;
						m_Machine_getPaths_DONE=false;
						m_selectedPosition++;
						m_FINISH_fusionOccured=true;
					}
				}
			}else{
				done=true;
			}
		}else{
			done=true;
		}
	}
	if(done){
		// there is nothing we can do.
		m_SEEDING_i++;
		m_FINISH_vertex_requested=false;
		m_EXTENSION_currentPosition=0;
		m_FUSION_pathLengthRequested=false;
		m_Machine_getPaths_INITIALIZED=false;
		m_Machine_getPaths_DONE=false;
		m_checkedValidity=false;
	}

}

void Machine::makeFusions(){
	// fusion.
	// find a path that matches directly.
	// if a path is 100% included in another, but the other is longer, keep the longest.
	// if a path is 100% identical to another one, keep the one with the lowest ID
	// if a path is 100% identical to another one, but is reverse-complement, keep the one with the lowest ID
	
	int END_LENGTH=100;
	// avoid duplication of contigs.
	if(m_SEEDING_i<(int)m_EXTENSION_contigs.size()){
		if((int)m_EXTENSION_contigs[m_SEEDING_i].size()<=END_LENGTH){
			END_LENGTH=m_EXTENSION_contigs[m_SEEDING_i].size()-1;
		}
	}
	if(m_SEEDING_i==(int)m_EXTENSION_contigs.size()){
		Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,MASTER_RANK,TAG_FUSION_DONE,getRank());
		m_outbox.push_back(aMessage);
		m_mode=MODE_DO_NOTHING;
		m_speedLimitIsOn=false;// remove the speed limit because rank MASTER will ask everyone their things
		#ifdef SHOW_PROGRESS
		cout<<"Rank "<<getRank()<<": fusion "<<m_SEEDING_i-1<<"/"<<m_EXTENSION_contigs.size()<<" (DONE)"<<endl;
		#endif
		#ifdef DEBUG
		//cout<<"Rank "<<getRank()<<" eliminated: "<<m_FUSION_eliminated.size()<<endl;
		#endif
		return;
	}else if((int)m_EXTENSION_contigs[m_SEEDING_i].size()<=END_LENGTH){
		#ifdef SHOW_PROGRESS
		cout<<"No fusion for me. "<<m_SEEDING_i<<" "<<m_EXTENSION_contigs[m_SEEDING_i].size()<<" "<<m_EXTENSION_identifiers[m_SEEDING_i]<<endl;
		#endif
		m_FUSION_direct_fusionDone=false;
		m_FUSION_first_done=false;
		m_FUSION_paths_requested=false;
		m_SEEDING_i++;
		return;
	}else if(!m_FUSION_direct_fusionDone){
		int currentId=m_EXTENSION_identifiers[m_SEEDING_i];
		if(!m_FUSION_first_done){
			if(!m_FUSION_paths_requested){
				#ifdef SHOW_PROGRESS
				if(m_SEEDING_i%100==0){
					cout<<"Rank "<<getRank()<<": fusion "<<m_SEEDING_i<<"/"<<m_EXTENSION_contigs.size()<<endl;
				}
				#endif
				// get the paths going on the first vertex
				#ifdef DEBUG
				assert((int)m_EXTENSION_contigs[m_SEEDING_i].size()>END_LENGTH);
				#endif
				VERTEX_TYPE theVertex=m_EXTENSION_contigs[m_SEEDING_i][END_LENGTH];
				VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
				message[0]=theVertex;
				Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(theVertex),TAG_ASK_VERTEX_PATHS_SIZE,getRank());
				m_outbox.push_back(aMessage);
				m_FUSION_paths_requested=true;
				m_FUSION_paths_received=false;
				m_FUSION_path_id=0;
				m_FUSION_path_requested=false;
			}else if(m_FUSION_paths_received){
				if(m_FUSION_path_id<m_FUSION_numberOfPaths){
					if(!m_FUSION_path_requested){
						VERTEX_TYPE theVertex=m_EXTENSION_contigs[m_SEEDING_i][END_LENGTH];
						VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
						message[0]=m_FUSION_path_id;
						Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(theVertex),TAG_ASK_VERTEX_PATH,getRank());
						m_outbox.push_back(aMessage);
						m_FUSION_path_requested=true;
						m_FUSION_path_received=false;
					}else if(m_FUSION_path_received){
						m_FUSION_path_id++;
						m_FUSION_receivedPaths.push_back(m_FUSION_receivedPath);
						m_FUSION_path_requested=false;
					}
				}else{
					m_FUSION_first_done=true;
					m_FUSION_paths_requested=false;
					m_FUSION_last_done=false;
					m_FUSION_firstPaths=m_FUSION_receivedPaths;
					#ifdef DEBUG
					assert(m_FUSION_numberOfPaths==(int)m_FUSION_firstPaths.size());
					#endif
				}
			}
		}else if(!m_FUSION_last_done){
			// get the paths going on the last vertex.

			if(!m_FUSION_paths_requested){
				// get the paths going on the lastvertex<
				#ifdef DEBUG
				assert((int)m_EXTENSION_contigs[m_SEEDING_i].size()>=END_LENGTH);
				#endif
				VERTEX_TYPE theVertex=m_EXTENSION_contigs[m_SEEDING_i][m_EXTENSION_contigs[m_SEEDING_i].size()-END_LENGTH];
				VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
				message[0]=theVertex;
				Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(theVertex),TAG_ASK_VERTEX_PATHS_SIZE,getRank());
				m_outbox.push_back(aMessage);
				m_FUSION_paths_requested=true;
				m_FUSION_paths_received=false;
				m_FUSION_path_id=0;
				m_FUSION_path_requested=false;
			}else if(m_FUSION_paths_received){
				if(m_FUSION_path_id<m_FUSION_numberOfPaths){
					if(!m_FUSION_path_requested){
						VERTEX_TYPE theVertex=m_EXTENSION_contigs[m_SEEDING_i][m_EXTENSION_contigs[m_SEEDING_i].size()-END_LENGTH];
						VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
						message[0]=m_FUSION_path_id;
						Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(theVertex),TAG_ASK_VERTEX_PATH,getRank());
						m_outbox.push_back(aMessage);
						m_FUSION_path_requested=true;
						m_FUSION_path_received=false;
					}else if(m_FUSION_path_received){
						m_FUSION_path_id++;
						m_FUSION_receivedPaths.push_back(m_FUSION_receivedPath);
						m_FUSION_path_requested=false;
					}
				}else{
					m_FUSION_last_done=true;
					m_FUSION_paths_requested=false;
					m_FUSION_lastPaths=m_FUSION_receivedPaths;
					m_FUSION_matches_done=false;
					m_FUSION_matches.clear();

					#ifdef DEBUG
					assert(m_FUSION_numberOfPaths==(int)m_FUSION_lastPaths.size());
					#endif
				}
			}


		}else if(!m_FUSION_matches_done){
			m_FUSION_matches_done=true;
			map<int,int> index;
			map<int,vector<int> > starts;
			map<int,vector<int> > ends;


			// extract those that are on both starting and ending vertices.
			for(int i=0;i<(int)m_FUSION_firstPaths.size();i++){
				index[m_FUSION_firstPaths[i].getWave()]++;
				int pathId=m_FUSION_firstPaths[i].getWave();
				int progression=m_FUSION_firstPaths[i].getProgression();
				starts[pathId].push_back(progression);
			}

			vector<int> matches;

			for(int i=0;i<(int)m_FUSION_lastPaths.size();i++){
				index[m_FUSION_lastPaths[i].getWave()]++;
				
				int pathId=m_FUSION_lastPaths[i].getWave();
				int progression=m_FUSION_lastPaths[i].getProgression();
				ends[pathId].push_back(progression);
			}
			

			
			for(map<int,int>::iterator i=index.begin();i!=index.end();++i){
				int otherPathId=i->first;
				if(i->second>=2 and otherPathId != currentId){
					// try to find a match with the current size.
					for(int k=0;k<(int)starts[otherPathId].size();k++){
						bool found=false;
						for(int p=0;p<(int)ends[otherPathId].size();p++){
							int observedLength=ends[otherPathId][p]-starts[otherPathId][k]+1;
							int expectedLength=m_EXTENSION_contigs[m_SEEDING_i].size()-2*END_LENGTH+1;
							//cout<<observedLength<<" versus "<<expectedLength<<endl;
							if(observedLength==expectedLength){
								m_FUSION_matches.push_back(otherPathId);
								found=true;
								break;
							}
						}
						if(found)
							break;
					}
				}
			}
			if(m_FUSION_matches.size()==0){ // no match, go next.
				m_FUSION_direct_fusionDone=true;
				m_FUSION_reverse_fusionDone=false;
				m_FUSION_first_done=false;
				m_FUSION_paths_requested=false;
			}
			m_FUSION_matches_length_done=false;
			m_FUSION_match_index=0;
			m_FUSION_pathLengthRequested=false;
		}else if(!m_FUSION_matches_length_done){
			int currentId=m_EXTENSION_identifiers[m_SEEDING_i];
			if(m_FUSION_match_index==(int)m_FUSION_matches.size()){// tested all matches, and nothing was found.
				m_FUSION_matches_length_done=true;
			}else if(!m_FUSION_pathLengthRequested){
				int uniquePathId=m_FUSION_matches[m_FUSION_match_index];
				int rankId=uniquePathId%MAX_NUMBER_OF_MPI_PROCESSES;
				VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(sizeof(VERTEX_TYPE));
				message[0]=uniquePathId;
				Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,rankId,TAG_GET_PATH_LENGTH,getRank());
				m_outbox.push_back(aMessage);
				m_FUSION_pathLengthRequested=true;
				m_FUSION_pathLengthReceived=false;
			}else if(m_FUSION_pathLengthReceived){
				if(m_FUSION_receivedLength==0){
				}else if(m_FUSION_matches[m_FUSION_match_index]<currentId and m_FUSION_receivedLength == (int)m_EXTENSION_contigs[m_SEEDING_i].size()){
					m_FUSION_eliminated.insert(currentId);
					m_FUSION_direct_fusionDone=false;
					m_FUSION_first_done=false;
					m_FUSION_paths_requested=false;
					m_SEEDING_i++;
				}else if(m_FUSION_receivedLength>(int)m_EXTENSION_contigs[m_SEEDING_i].size() ){
					m_FUSION_eliminated.insert(currentId);
					m_FUSION_direct_fusionDone=false;
					m_FUSION_first_done=false;
					m_FUSION_paths_requested=false;
					m_SEEDING_i++;
				}
				m_FUSION_match_index++;
				m_FUSION_pathLengthRequested=false;
			}
		}else if(m_FUSION_matches_length_done){ // no candidate found for fusion.
			m_FUSION_direct_fusionDone=true;
			m_FUSION_reverse_fusionDone=false;
			m_FUSION_first_done=false;
			m_FUSION_paths_requested=false;
		}
	}else if(!m_FUSION_reverse_fusionDone){
		int currentId=m_EXTENSION_identifiers[m_SEEDING_i];
		if(!m_FUSION_first_done){
			if(!m_FUSION_paths_requested){
				// get the paths going on the first vertex
				VERTEX_TYPE theVertex=complementVertex(m_EXTENSION_contigs[m_SEEDING_i][m_EXTENSION_contigs[m_SEEDING_i].size()-END_LENGTH],m_wordSize,m_colorSpaceMode);
				VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
				message[0]=theVertex;
				Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(theVertex),TAG_ASK_VERTEX_PATHS_SIZE,getRank());
				m_outbox.push_back(aMessage);
				m_FUSION_paths_requested=true;
				m_FUSION_paths_received=false;
				m_FUSION_path_id=0;
				m_FUSION_path_requested=false;
			}else if(m_FUSION_paths_received){
				if(m_FUSION_path_id<m_FUSION_numberOfPaths){
					if(!m_FUSION_path_requested){
						VERTEX_TYPE theVertex=complementVertex(m_EXTENSION_contigs[m_SEEDING_i][m_EXTENSION_contigs[m_SEEDING_i].size()-END_LENGTH],m_wordSize,m_colorSpaceMode);
						VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
						message[0]=m_FUSION_path_id;
						Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(theVertex),TAG_ASK_VERTEX_PATH,getRank());
						m_outbox.push_back(aMessage);
						m_FUSION_path_requested=true;
						m_FUSION_path_received=false;
					}else if(m_FUSION_path_received){
						m_FUSION_path_id++;
						m_FUSION_receivedPaths.push_back(m_FUSION_receivedPath);
						m_FUSION_path_requested=false;
					}
				}else{
					m_FUSION_first_done=true;
					m_FUSION_paths_requested=false;
					m_FUSION_last_done=false;
					m_FUSION_firstPaths=m_FUSION_receivedPaths;
					#ifdef DEBUG
					assert(m_FUSION_numberOfPaths==(int)m_FUSION_firstPaths.size());
					#endif
				}
			}
		}else if(!m_FUSION_last_done){
			// get the paths going on the last vertex.

			if(!m_FUSION_paths_requested){
				// get the paths going on the first vertex
				VERTEX_TYPE theVertex=complementVertex(m_EXTENSION_contigs[m_SEEDING_i][END_LENGTH],m_wordSize,m_colorSpaceMode);
				VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
				message[0]=theVertex;
				Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(theVertex),TAG_ASK_VERTEX_PATHS_SIZE,getRank());
				m_outbox.push_back(aMessage);
				m_FUSION_paths_requested=true;
				m_FUSION_paths_received=false;
				m_FUSION_path_id=0;
				m_FUSION_path_requested=false;
			}else if(m_FUSION_paths_received){
				if(m_FUSION_path_id<m_FUSION_numberOfPaths){
					if(!m_FUSION_path_requested){
						VERTEX_TYPE theVertex=complementVertex(m_EXTENSION_contigs[m_SEEDING_i][END_LENGTH],m_wordSize,m_colorSpaceMode);
						VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
						message[0]=m_FUSION_path_id;
						Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(theVertex),TAG_ASK_VERTEX_PATH,getRank());
						m_outbox.push_back(aMessage);
						m_FUSION_path_requested=true;
						m_FUSION_path_received=false;
					}else if(m_FUSION_path_received){
						m_FUSION_path_id++;
						m_FUSION_receivedPaths.push_back(m_FUSION_receivedPath);
						m_FUSION_path_requested=false;
					}
				}else{
					m_FUSION_last_done=true;
					m_FUSION_paths_requested=false;
					m_FUSION_lastPaths=m_FUSION_receivedPaths;
					m_FUSION_matches_done=false;
					m_FUSION_matches.clear();

					#ifdef DEBUG
					assert(m_FUSION_numberOfPaths==(int)m_FUSION_lastPaths.size());
					#endif
				}
			}



		}else if(!m_FUSION_matches_done){
			m_FUSION_matches_done=true;
			map<int,int> index;
			map<int,vector<int> > starts;
			map<int,vector<int> > ends;
			for(int i=0;i<(int)m_FUSION_firstPaths.size();i++){
				index[m_FUSION_firstPaths[i].getWave()]++;
				int pathId=m_FUSION_firstPaths[i].getWave();
				int progression=m_FUSION_firstPaths[i].getProgression();
				starts[pathId].push_back(progression);
			}
			for(int i=0;i<(int)m_FUSION_lastPaths.size();i++){
				index[m_FUSION_lastPaths[i].getWave()]++;
				
				int pathId=m_FUSION_lastPaths[i].getWave();
				int progression=m_FUSION_lastPaths[i].getProgression();
				ends[pathId].push_back(progression);
			}
			vector<int> matches;
			for(map<int,int>::iterator i=index.begin();i!=index.end();++i){
				int otherPathId=i->first;
				if(i->second>=2 and i->first != currentId){
					// try to find a match with the current size.
					for(int k=0;k<(int)starts[otherPathId].size();k++){
						bool found=false;
						for(int p=0;p<(int)ends[otherPathId].size();p++){
							int observedLength=ends[otherPathId][p]-starts[otherPathId][k]+1;
							int expectedLength=m_EXTENSION_contigs[m_SEEDING_i].size()-2*END_LENGTH+1;
							//cout<<observedLength<<" versus "<<expectedLength<<endl;
							if(observedLength==expectedLength){
								m_FUSION_matches.push_back(otherPathId);
								found=true;
								break;
							}
						}
						if(found)
							break;
					}
				}
			}
			if(m_FUSION_matches.size()==0){ // no match, go next.
				m_FUSION_direct_fusionDone=false;
				m_FUSION_first_done=false;
				m_FUSION_paths_requested=false;
				m_SEEDING_i++;
			}
			m_FUSION_matches_length_done=false;
			m_FUSION_match_index=0;
			m_FUSION_pathLengthRequested=false;
		}else if(!m_FUSION_matches_length_done){
			int currentId=m_EXTENSION_identifiers[m_SEEDING_i];
			if(m_FUSION_match_index==(int)m_FUSION_matches.size()){
				m_FUSION_matches_length_done=true;
			}else if(!m_FUSION_pathLengthRequested){
				int uniquePathId=m_FUSION_matches[m_FUSION_match_index];
				int rankId=uniquePathId%MAX_NUMBER_OF_MPI_PROCESSES;
				VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(sizeof(VERTEX_TYPE));
				message[0]=uniquePathId;
				Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,rankId,TAG_GET_PATH_LENGTH,getRank());
				m_outbox.push_back(aMessage);
				m_FUSION_pathLengthRequested=true;
				m_FUSION_pathLengthReceived=false;
			}else if(m_FUSION_pathLengthReceived){
				if(m_FUSION_receivedLength==0){
				}else if(m_FUSION_matches[m_FUSION_match_index]<currentId and m_FUSION_receivedLength == (int)m_EXTENSION_contigs[m_SEEDING_i].size()){
					m_FUSION_eliminated.insert(currentId);
					m_FUSION_direct_fusionDone=false;
					m_FUSION_first_done=false;
					m_FUSION_paths_requested=false;
					m_SEEDING_i++;
				}else if(m_FUSION_receivedLength>(int)m_EXTENSION_contigs[m_SEEDING_i].size()){
					m_FUSION_eliminated.insert(currentId);
					m_FUSION_direct_fusionDone=false;
					m_FUSION_first_done=false;
					m_FUSION_paths_requested=false;
					m_SEEDING_i++;
				}
				m_FUSION_match_index++;
				m_FUSION_pathLengthRequested=false;
			}
		}else if(m_FUSION_matches_length_done){ // no candidate found for fusion.
			m_FUSION_direct_fusionDone=false;
			m_FUSION_first_done=false;
			m_FUSION_paths_requested=false;
			m_SEEDING_i++;
		}
	}
}

void Machine::processMessages(){
	for(int i=0;i<(int)m_inbox.size();i++){
		processMessage(&(m_inbox[i]));
	}
	m_inbox.clear();
	m_inboxAllocator.reset();
}

void Machine::processData(){
	if(m_aborted){
		return;
	}
	
	#ifndef SHOW_PROGRESS
	if(m_readyToSeed==getSize()){
		cout<<"\r"<<"Computing seeds"<<endl;
	}

	if(isMaster() and m_messageSentForVerticesDistribution and m_numberOfMachinesDoneSendingVertices<getSize()){
		time_t tmp=time(NULL);
		if(tmp>m_lastTime){
			m_lastTime=tmp;
			showProgress();
		}
	}else if(isMaster() and m_messageSentForEdgesDistribution and m_numberOfMachinesDoneSendingEdges<getSize() and m_numberOfMachinesDoneSendingEdges!=-9){
		time_t tmp=time(NULL);
		if(tmp>m_lastTime){
			m_lastTime=tmp;
			showProgress();
		}
	}else if(isMaster() and m_fusionStarted  and m_FUSION_numberOfRanksDone<getSize()){
		time_t tmp=time(NULL);
		if(tmp>m_lastTime){
			m_lastTime=tmp;
			showProgress();
		}

	}
	#endif

	if(!m_parameters.isInitiated()&&isMaster()){
		if(m_argc==2){
			ifstream f(m_argv[1]);
			if(!f){
				cout<<"Rank "<<getRank()<<" invalid input file."<<endl;
				m_aborted=true;
				f.close();
				killRanks();
				return;
			}
		}
		m_parameters.load(m_argc,m_argv);
		if(m_parameters.useAmos()){
			// empty the file.
			cout<<"Preparing AMOS file "<<m_parameters.getAmosFile()<<endl;
			m_bubbleData->m_amos=fopen(m_parameters.getAmosFile().c_str(),"w");
		}

		for(int i=0;i<getSize();i++){
			VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
			message[0]=m_parameters.getWordSize();
			Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,i,TAG_SET_WORD_SIZE,getRank());
			m_outbox.push_back(aMessage);
			
			VERTEX_TYPE*message2=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));

			message2[0]=m_parameters.getColorSpaceMode();
			Message aMessage2(message2,1,MPI_UNSIGNED_LONG_LONG,i,TAG_SET_COLOR_MODE,getRank());
			m_outbox.push_back(aMessage2);
		}
	}else if(m_welcomeStep==true && m_loadSequenceStep==false&&isMaster()){
		loadSequences();
	}else if(m_loadSequenceStep==true && m_mode_send_vertices==false&&isMaster() and m_sequence_ready_machines==getSize()&&m_messageSentForVerticesDistribution==false){
		#ifdef SHOW_PROGRESS
		cout<<"Rank "<<getRank()<<": starting vertices distribution."<<endl;
		#else
		cout<<"\r"<<"Computing vertices"<<endl;
		#endif
		for(int i=0;i<getSize();i++){
			Message aMessage(NULL, 0, MPI_UNSIGNED_LONG_LONG,i, TAG_START_VERTICES_DISTRIBUTION,getRank());
			m_outbox.push_back(aMessage);
		}
		m_messageSentForVerticesDistribution=true;
	}else if(m_numberOfMachinesDoneSendingVertices==getSize()){
		m_numberOfMachinesReadyForEdgesDistribution=0;
		m_numberOfMachinesDoneSendingVertices=-1;
		for(int i=0;i<getSize();i++){
			Message aMessage(NULL, 0, MPI_UNSIGNED_LONG_LONG, i, TAG_SHOW_VERTICES,getRank());
			m_outbox.push_back(aMessage);
		}


		m_mode_AttachSequences=true;
		m_distribution_file_id=m_distribution_sequence_id=m_distribution_currentSequenceId=0;
		m_startEdgeDistribution=false;
	}else if(m_startEdgeDistribution){
		#ifndef SHOW_PROGRESS
		cout<<"\r"<<"Adding arcs"<<endl;
		#endif
		for(int i=0;i<getSize();i++){
			Message aMessage(NULL, 0, MPI_UNSIGNED_LONG_LONG,i, TAG_START_EDGES_DISTRIBUTION_ASK,getRank());
			m_outbox.push_back(aMessage);
		}
		m_startEdgeDistribution=false;
	}else if(m_numberOfMachinesReadyForEdgesDistribution==getSize() and isMaster()){
		m_numberOfMachinesReadyForEdgesDistribution=-1;
		for(int i=0;i<getSize();i++){
			Message aMessage(NULL, 0, MPI_UNSIGNED_LONG_LONG,i, TAG_START_EDGES_DISTRIBUTION,getRank());
			m_outbox.push_back(aMessage);
		}
		m_messageSentForEdgesDistribution=true;
	}else if(m_numberOfMachinesDoneSendingCoverage==getSize()){
		m_numberOfMachinesDoneSendingCoverage=-1;
		CoverageDistribution distribution(m_coverageDistribution,m_parameters.getCoverageDistributionFile());
		m_minimumCoverage=distribution.getMinimumCoverage();
		m_peakCoverage=distribution.getPeakCoverage();
		m_seedCoverage=(m_minimumCoverage+m_peakCoverage)/2;

		m_coverageDistribution.clear();

		COVERAGE_TYPE maxCoverage=0;
		maxCoverage--;// underflow.
		#ifdef SHOW_PROGRESS
		cout<<"MaxCoverage="<<(int)maxCoverage<<endl;
		#endif
		ofstream f(m_parameters.getParametersFile().c_str());
		f<<"Ray Command Line: ";
		vector<string> commands=m_parameters.getCommands();
		for(int i=0;i<(int)commands.size();i++){
			f<<" "<<commands[i];
		}
		f<<endl;
		f<<"Ray Assembly Engine: "<<m_parameters.getEngineName()<<"-"<<m_parameters.getVersion()<<endl;
		f<<"Ray Word Size: "<<m_wordSize<<endl;
		f<<"Ray Minimum Coverage: "<<m_minimumCoverage<<endl;
		f<<"Ray Peak Coverage: "<<m_peakCoverage<<endl;
		f<<"Ray Contigs File: "<<m_parameters.getContigsFile()<<endl;
		if(m_parameters.useAmos()){
			f<<"Ray AMOS File: "<<m_parameters.getAmosFile()<<endl;
		}
		f<<"Ray Parameters File: "<<m_parameters.getParametersFile()<<endl;
		f<<"Ray Coverage Distribution File: "<<m_parameters.getCoverageDistributionFile()<<endl;

		f.close();
		cout<<"Writing "<<m_parameters.getParametersFile()<<""<<endl;

		if(m_minimumCoverage > m_peakCoverage or m_peakCoverage==maxCoverage){
			killRanks();
			cout<<"Error: no enrichment observed."<<endl;
			return;
		}

		// see these values to everyone.
		VERTEX_TYPE*buffer=(VERTEX_TYPE*)m_outboxAllocator.allocate(3*sizeof(VERTEX_TYPE));
		buffer[0]=m_minimumCoverage;
		buffer[1]=m_seedCoverage;
		buffer[2]=m_peakCoverage;
		for(int i=0;i<getSize();i++){
			Message aMessage(buffer,3,MPI_UNSIGNED_LONG_LONG,i,TAG_SEND_COVERAGE_VALUES,getRank());
			m_outbox.push_back(aMessage);
		}
	}else if(m_mode_send_vertices==true){
		#ifdef SHOW_PROGRESS
		if(m_mode_send_vertices_sequence_id%100000==0 and m_mode_send_vertices_sequence_id_position==0){
			string reverse="";
			if(m_reverseComplementVertex==true)
				reverse="(reverse complement) ";
			cout<<"Rank "<<getRank()<<" is extracting vertices "<<reverse<<"from sequences "<<m_mode_send_vertices_sequence_id<<"/"<<m_myReads.size()<<endl;
		}
		#endif

		if(m_mode_send_vertices_sequence_id>(int)m_myReads.size()-1){
			if(m_reverseComplementVertex==false){
				#ifdef SHOW_PROGRESS
				cout<<"Rank "<<getRank()<<" is extracting vertices from sequences "<<m_mode_send_vertices_sequence_id<<"/"<<m_myReads.size()<<" (DONE)"<<endl;
				#endif
				m_mode_send_vertices_sequence_id=0;
				m_mode_send_vertices_sequence_id_position=0;
				m_reverseComplementVertex=true;
			}else{
				Message aMessage(NULL, 0, MPI_UNSIGNED_LONG_LONG, MASTER_RANK, TAG_VERTICES_DISTRIBUTED,getRank());
				m_outbox.push_back(aMessage);
				m_mode_send_vertices=false;
				#ifdef SHOW_PROGRESS
				cout<<"Rank "<<getRank()<<" is extracting vertices (reverse complement) from sequences "<<m_mode_send_vertices_sequence_id<<"/"<<m_myReads.size()<<" (DONE)"<<endl;
				#endif
			}
		}else{
			char*readSequence=m_myReads[m_mode_send_vertices_sequence_id]->getSeq();
			int len=strlen(readSequence);
			char memory[100];
			int lll=len-m_wordSize;
	
			map<int,vector<VERTEX_TYPE> > messagesStock;
			for(int p=m_mode_send_vertices_sequence_id_position;p<=m_mode_send_vertices_sequence_id_position;p++){
				memcpy(memory,readSequence+p,m_wordSize);
				memory[m_wordSize]='\0';
				if(isValidDNA(memory)){
					VERTEX_TYPE a=wordId(memory);
					if(m_reverseComplementVertex==false){
						messagesStock[vertexRank(a)].push_back(a);
					}else{
						VERTEX_TYPE b=complementVertex(a,m_wordSize,m_colorSpaceMode);
						messagesStock[vertexRank(b)].push_back(b);
					}
				}
			}
			m_mode_send_vertices_sequence_id_position++;
			// send messages
			for(map<int,vector<VERTEX_TYPE> >::iterator i=messagesStock.begin();i!=messagesStock.end();i++){
				int destination=i->first;
				int length=i->second.size();
				VERTEX_TYPE *data=(VERTEX_TYPE*)m_outboxAllocator.allocate(sizeof(VERTEX_TYPE)*length);
				for(int j=0;j<(int)i->second.size();j++){
					data[j]=i->second[j];
				}
				
				Message aMessage(data, length, MPI_UNSIGNED_LONG_LONG,destination, TAG_VERTICES_DATA,getRank());
				m_outbox.push_back(aMessage);
				m_vertices_sent+=length;
			}

			if(m_mode_send_vertices_sequence_id_position>lll){
				m_mode_send_vertices_sequence_id++;
				m_mode_send_vertices_sequence_id_position=0;
			}
		}
		
	}else if(m_numberOfMachinesDoneSendingEdges==getSize()){
		m_numberOfMachinesDoneSendingEdges=-9;
		for(int i=0;i<getSize();i++){
			Message aMessage(NULL, 0, MPI_UNSIGNED_LONG_LONG, i, TAG_PREPARE_COVERAGE_DISTRIBUTION_QUESTION,getRank());
			m_outbox.push_back(aMessage);
		}
	}else if(m_numberOfMachinesReadyToSendDistribution==getSize()){
		if(m_machineRank<=m_numberOfMachinesDoneSendingCoverage){
			Message aMessage(NULL, 0, MPI_UNSIGNED_LONG_LONG, m_machineRank, TAG_PREPARE_COVERAGE_DISTRIBUTION,getRank());
			m_outbox.push_back(aMessage);
			m_machineRank++;
		}

		if(m_machineRank==getSize()){
			m_numberOfMachinesReadyToSendDistribution=-1;
		}
	}else if(m_ranksDoneAttachingReads==getSize()){
		m_ranksDoneAttachingReads=-1;
		m_startEdgeDistribution=true;
	}

	if(m_mode==MODE_ASSEMBLE_WAVES){
		// take each seed, and extend it in both direction using previously obtained information.
		if(m_SEEDING_i==(int)m_SEEDING_seeds.size()){
			Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,MASTER_RANK,TAG_ASSEMBLE_WAVES_DONE,getRank());
			m_outbox.push_back(aMessage);
		}else{
		}
	}else if(m_mode==MODE_PERFORM_CALIBRATION){
		int rank=rand()%getSize();
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
		Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,rank,TAG_CALIBRATION_MESSAGE,getRank());
		m_outbox.push_back(aMessage);
		m_calibration_numberOfMessagesSent++;
	}else if(m_mode==MODE_FINISH_FUSIONS){
		finishFusions();
	}else if(m_mode==MODE_DISTRIBUTE_FUSIONS){
		if(m_SEEDING_i==(int)m_EXTENSION_contigs.size()){
			Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,MASTER_RANK,TAG_DISTRIBUTE_FUSIONS_FINISHED,getRank());
			m_outbox.push_back(aMessage);
			m_mode=MODE_DO_NOTHING;
			return;
		}
	
		VERTEX_TYPE vertex=m_EXTENSION_contigs[m_SEEDING_i][m_EXTENSION_currentPosition];
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(3*sizeof(VERTEX_TYPE));
		message[0]=vertex;
		message[1]=m_EXTENSION_identifiers[m_SEEDING_i];
		message[2]=m_EXTENSION_currentPosition;
		Message aMessage(message,3,MPI_UNSIGNED_LONG_LONG,vertexRank(vertex),TAG_SAVE_WAVE_PROGRESSION,getRank());
		m_outbox.push_back(aMessage);

		m_EXTENSION_currentPosition++;

		// the next one
		if(m_EXTENSION_currentPosition==(int)m_EXTENSION_contigs[m_SEEDING_i].size()){
			m_SEEDING_i++;
			m_EXTENSION_currentPosition=0;
		}
	}

	if(m_mode_sendDistribution){
		if(m_distributionOfCoverage.size()==0){
			SplayTreeIterator<VERTEX_TYPE,Vertex> iterator(&m_subgraph);
			while(iterator.hasNext()){
				int coverage=iterator.next()->getValue()->getCoverage();
				m_distributionOfCoverage[coverage]++;
			}
		}

		int length=2;
		VERTEX_TYPE*data=(VERTEX_TYPE*)m_outboxAllocator.allocate(sizeof(VERTEX_TYPE)*length);
		int j=0;
		for(map<int,VERTEX_TYPE>::iterator i=m_distributionOfCoverage.begin();i!=m_distributionOfCoverage.end();i++){
			int coverage=i->first;
			VERTEX_TYPE count=i->second;
			if(m_mode_send_coverage_iterator==j){
				data[0]=coverage;
				data[1]=count;
				break;
			}
			j++;
		}
		Message aMessage(data,length, MPI_UNSIGNED_LONG_LONG, MASTER_RANK, TAG_COVERAGE_DATA,getRank());
		m_outbox.push_back(aMessage);

		m_mode_send_coverage_iterator++;
		if(m_mode_send_coverage_iterator>=(int)m_distributionOfCoverage.size()){
			m_mode_sendDistribution=false;
			Message aMessage(NULL, 0, MPI_UNSIGNED_LONG_LONG,MASTER_RANK, TAG_COVERAGE_END,getRank());
			m_outbox.push_back(aMessage);
			m_distributionOfCoverage.clear();
		}

	}else if(m_mode_send_outgoing_edges==true){ 
		#ifdef SHOW_PROGRESS
		if(m_mode_send_edge_sequence_id%100000==0 and m_mode_send_edge_sequence_id_position==0){
			string strand="";
			if(m_reverseComplementEdge)
				strand="(reverse complement)";
			cout<<"Rank "<<getRank()<<" is extracting outgoing edges "<<strand<<" "<<m_mode_send_edge_sequence_id<<"/"<<m_myReads.size()<<endl;
		}
		#endif

		if(m_mode_send_edge_sequence_id>(int)m_myReads.size()-1){
			if(m_reverseComplementEdge==false){
				m_mode_send_edge_sequence_id_position=0;
				m_reverseComplementEdge=true;
				#ifdef SHOW_PROGRESS
				cout<<"Rank "<<getRank()<<" is extracting outgoing edges "<<m_mode_send_edge_sequence_id<<"/"<<m_myReads.size()<<" (DONE)"<<endl;
				#endif
				m_mode_send_edge_sequence_id=0;
			}else{
				m_mode_send_outgoing_edges=false;
				m_mode_send_ingoing_edges=true;
				m_mode_send_edge_sequence_id_position=0;
				#ifdef SHOW_PROGRESS
				cout<<"Rank "<<getRank()<<" is extracting outgoing edges (reverse complement) "<<m_mode_send_edge_sequence_id<<"/"<<m_myReads.size()<<" (DONE)"<<endl;
				#endif
				m_mode_send_edge_sequence_id=0;
				m_reverseComplementEdge=false;
			}
		}else{
			char*readSequence=m_myReads[m_mode_send_edge_sequence_id]->getSeq();
			int len=strlen(readSequence);

			char memory[100];
			int lll=len-m_wordSize-1;
			if(m_mode_send_edge_sequence_id_position>lll){
				m_mode_send_edge_sequence_id++;
				m_mode_send_edge_sequence_id_position=0;
				return;
			}

			map<int,vector<VERTEX_TYPE> > messagesStockOut;
			for(int p=m_mode_send_edge_sequence_id_position;p<=m_mode_send_edge_sequence_id_position;p++){
				memcpy(memory,readSequence+p,m_wordSize+1);
				memory[m_wordSize+1]='\0';
				if(isValidDNA(memory)){
					char prefix[100];
					char suffix[100];
					strcpy(prefix,memory);
					prefix[m_wordSize]='\0';
					strcpy(suffix,memory+1);
					VERTEX_TYPE a_1=wordId(prefix);
					VERTEX_TYPE a_2=wordId(suffix);
					if(m_reverseComplementEdge){
						VERTEX_TYPE b_1=complementVertex(a_2,m_wordSize,m_colorSpaceMode);
						VERTEX_TYPE b_2=complementVertex(a_1,m_wordSize,m_colorSpaceMode);

						int rankB=vertexRank(b_1);
						messagesStockOut[rankB].push_back(b_1);
						messagesStockOut[rankB].push_back(b_2);
					}else{
						int rankA=vertexRank(a_1);
						messagesStockOut[rankA].push_back(a_1);
						messagesStockOut[rankA].push_back(a_2);
					}
					
				}
			}

			m_mode_send_edge_sequence_id_position++;

			for(map<int,vector<VERTEX_TYPE> >::iterator i=messagesStockOut.begin();i!=messagesStockOut.end();i++){
				int destination=i->first;
				int length=i->second.size();
				VERTEX_TYPE*data=(VERTEX_TYPE*)m_outboxAllocator.allocate(sizeof(VERTEX_TYPE)*(length));
				for(int j=0;j<(int)i->second.size();j++){
					data[j]=i->second[j];
				}
	
				Message aMessage(data, length, MPI_UNSIGNED_LONG_LONG,destination, TAG_OUT_EDGES_DATA,getRank());
				m_outbox.push_back(aMessage);
			}


		}
	}else if(m_mode_send_ingoing_edges==true){ 

		#ifdef SHOW_PROGRESS
		if(m_mode_send_edge_sequence_id%100000==0 and m_mode_send_edge_sequence_id_position==0){
			string strand="";
			if(m_reverseComplementEdge)
				strand="(reverse complement)";
			cout<<"Rank "<<getRank()<<" is extracting ingoing edges "<<strand<<" "<<m_mode_send_edge_sequence_id<<"/"<<m_myReads.size()<<endl;
		}
		#endif

		if(m_mode_send_edge_sequence_id>(int)m_myReads.size()-1){
			if(m_reverseComplementEdge==false){
				m_reverseComplementEdge=true;
				m_mode_send_edge_sequence_id_position=0;
				#ifdef SHOW_PROGRESS
				cout<<"Rank "<<getRank()<<" is extracting ingoing edges "<<m_mode_send_edge_sequence_id<<"/"<<m_myReads.size()<<" (DONE)"<<endl;
				#endif
				m_mode_send_edge_sequence_id=0;
			}else{
				Message aMessage(NULL,0, MPI_UNSIGNED_LONG_LONG, MASTER_RANK, TAG_EDGES_DISTRIBUTED,getRank());
				m_outbox.push_back(aMessage);
				m_mode_send_ingoing_edges=false;
			
				#ifdef SHOW_PROGRESS
				cout<<"Rank "<<getRank()<<" is extracting ingoing edges (reverse complement) "<<m_mode_send_edge_sequence_id<<"/"<<m_myReads.size()<<" (DONE)"<<endl;
				#endif
			}
		}else{


			char*readSequence=m_myReads[m_mode_send_edge_sequence_id]->getSeq();
			int len=strlen(readSequence);
			char memory[100];
			int lll=len-m_wordSize-1;
			
			if(m_mode_send_edge_sequence_id_position>lll){
				m_mode_send_edge_sequence_id++;
				m_mode_send_edge_sequence_id_position=0;
				return;
			}

			map<int,vector<VERTEX_TYPE> > messagesStockIn;
			for(int p=m_mode_send_edge_sequence_id_position;p<=m_mode_send_edge_sequence_id_position;p++){
				memcpy(memory,readSequence+p,m_wordSize+1);
				memory[m_wordSize+1]='\0';
				if(isValidDNA(memory)){
					char prefix[100];
					char suffix[100];
					strcpy(prefix,memory);
					prefix[m_wordSize]='\0';
					strcpy(suffix,memory+1);
					VERTEX_TYPE a_1=wordId(prefix);
					VERTEX_TYPE a_2=wordId(suffix);
					if(m_reverseComplementEdge){
						VERTEX_TYPE b_1=complementVertex(a_2,m_wordSize,m_colorSpaceMode);
						VERTEX_TYPE b_2=complementVertex(a_1,m_wordSize,m_colorSpaceMode);
						int rankB=vertexRank(b_2);
						messagesStockIn[rankB].push_back(b_1);
						messagesStockIn[rankB].push_back(b_2);
					}else{
						int rankA=vertexRank(a_2);
						messagesStockIn[rankA].push_back(a_1);
						messagesStockIn[rankA].push_back(a_2);
					}
				}
			}

			m_mode_send_edge_sequence_id_position++;

			// send messages
			for(map<int,vector<VERTEX_TYPE> >::iterator i=messagesStockIn.begin();i!=messagesStockIn.end();i++){
				int destination=i->first;
				int length=i->second.size();
				VERTEX_TYPE*data=(VERTEX_TYPE*)m_outboxAllocator.allocate(sizeof(VERTEX_TYPE)*(length));
				for(int j=0;j<(int)i->second.size();j++){
					data[j]=i->second[j];
				}
	
				Message aMessage(data, length, MPI_UNSIGNED_LONG_LONG,destination, TAG_IN_EDGES_DATA,getRank());
				m_outbox.push_back(aMessage);
			}

			if(m_mode_send_edge_sequence_id_position>lll){
				m_mode_send_edge_sequence_id++;
				m_mode_send_edge_sequence_id_position=0;
			}
		}
	}else if(m_readyToSeed==getSize()){
		m_readyToSeed=-1;
		m_numberOfRanksDoneSeeding=0;
		// tell everyone to seed now.
		for(int i=0;i<getSize();i++){
			Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,i,TAG_START_SEEDING,getRank());
			m_outbox.push_back(aMessage);
		}
	}else if(m_mode==MODE_START_SEEDING){

		// assign a first vertex
		if(!m_SEEDING_NodeInitiated){
			if(m_SEEDING_i==(int)m_subgraph.size()-1){
				m_mode=MODE_DO_NOTHING;
				#ifdef SHOW_PROGRESS
				cout<<"Rank "<<getRank()<<" seeding vertices. "<<m_SEEDING_i<<"/"<<m_subgraph.size()<<" (DONE)"<<endl;
				#endif
				Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,MASTER_RANK,TAG_SEEDING_IS_OVER,getRank());
				m_SEEDING_nodes.clear();
				m_outbox.push_back(aMessage);
			}else{
				#ifdef SHOW_PROGRESS
				if(m_SEEDING_i % 100000 ==0){
					cout<<"Rank "<<getRank()<<" seeding vertices. "<<m_SEEDING_i<<"/"<<m_subgraph.size()<<endl;
				}
				#endif
				m_SEEDING_currentVertex=m_SEEDING_nodes[m_SEEDING_i];
				m_SEEDING_first=m_SEEDING_currentVertex;
				m_SEEDING_testInitiated=false;
				m_SEEDING_1_1_test_done=false;
				m_SEEDING_i++;
				m_SEEDING_NodeInitiated=true;
				m_SEEDING_firstVertexTestDone=false;
			}
		// check that this node has 1 ingoing edge and 1 outgoing edge.
		}else if(!m_SEEDING_firstVertexTestDone){
			if(!m_SEEDING_1_1_test_done){
				do_1_1_test();
			}else{
				if(!m_SEEDING_1_1_test_result){
					m_SEEDING_NodeInitiated=false;// abort
				}else{
					m_SEEDING_firstVertexParentTestDone=false;
					m_SEEDING_firstVertexTestDone=true;
					m_SEEDING_currentVertex=m_SEEDING_currentParentVertex;
					m_SEEDING_testInitiated=false;
					m_SEEDING_1_1_test_done=false;
				}
			}

		// check that the parent does not have 1 ingoing edge and 1 outgoing edge
		}else if(!m_SEEDING_firstVertexParentTestDone){
			if(!m_SEEDING_1_1_test_done){
				do_1_1_test();
			}else{
				if(m_SEEDING_1_1_test_result){
					m_SEEDING_NodeInitiated=false;//abort
				}else{
					m_SEEDING_firstVertexParentTestDone=true;
					m_SEEDING_vertices.clear();
					m_SEEDING_seed.clear();
					// restore original starter.
					m_SEEDING_currentVertex=m_SEEDING_first;
					m_SEEDING_testInitiated=false;
					m_SEEDING_1_1_test_done=false;
				}
			}

		// check if currentVertex has 1 ingoing edge and 1 outgoing edge, if yes, add it
		}else{
			// attempt to add m_SEEDING_currentVertex
			if(!m_SEEDING_1_1_test_done){
				do_1_1_test();
			}else{
				if(m_SEEDING_vertices.count(m_SEEDING_currentVertex)>0){
					m_SEEDING_1_1_test_result=false;
				}
				if(!m_SEEDING_1_1_test_result){
					m_SEEDING_NodeInitiated=false;
					int nucleotides=m_SEEDING_seed.size()+m_wordSize-1;
					// only consider the long ones.
					if(nucleotides>=m_parameters.getMinimumContigLength()){
						m_SEEDING_seeds.push_back(m_SEEDING_seed);
					}
				}else{
					m_SEEDING_seed.push_back(m_SEEDING_currentVertex);
					m_SEEDING_vertices.insert(m_SEEDING_currentVertex);
					m_SEEDING_currentVertex=m_SEEDING_currentChildVertex;
					m_SEEDING_testInitiated=false;
					m_SEEDING_1_1_test_done=false;
				}
			}
		}
	}else if(m_numberOfRanksDoneSeeding==getSize()){
		m_numberOfRanksDoneSeeding=-1;
		#ifndef SHOW_PROGRESS
		cout<<"\r"<<"Extending seeds"<<endl;
		#endif
		m_mode=MODE_EXTENSION_ASK;
		m_EXTENSION_rank=-1;
		m_EXTENSION_currentRankIsSet=false;
	}else if(m_mode_AttachSequences){
		attachReads();
	}else if(m_mode==MODE_EXTENSION_ASK and isMaster()){
		
		if(!m_EXTENSION_currentRankIsSet){
			m_EXTENSION_currentRankIsSet=true;
			m_EXTENSION_currentRankIsStarted=false;
			m_EXTENSION_rank++;
		}
		if(m_EXTENSION_rank==getSize()){
			m_mode=MODE_DO_NOTHING;

		}else if(!m_EXTENSION_currentRankIsStarted){
			m_EXTENSION_currentRankIsStarted=true;
			Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,m_EXTENSION_rank,TAG_ASK_EXTENSION,getRank());
			m_outbox.push_back(aMessage);
			m_EXTENSION_currentRankIsDone=true; // set to false for non-parallel extension.
		}else if(m_EXTENSION_currentRankIsDone){
			m_EXTENSION_currentRankIsSet=false;
		}

	}else if(m_mode==MODE_SEND_EXTENSION_DATA){
		if(m_SEEDING_i==(int)m_EXTENSION_contigs.size()){
			m_mode=MODE_DO_NOTHING;
			Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,MASTER_RANK,TAG_EXTENSION_DATA_END,getRank());
			m_outbox.push_back(aMessage);
		}else{
			if(m_FUSION_eliminated.count(m_EXTENSION_identifiers[m_SEEDING_i])>0){ // skip merged paths.
				m_SEEDING_i++;
				m_EXTENSION_currentPosition=0;
			}else{
				if(m_EXTENSION_currentPosition==0){
					VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(sizeof(VERTEX_TYPE)*1);
					int theId=m_EXTENSION_identifiers[m_SEEDING_i];
					message[0]=theId;
					Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,MASTER_RANK,TAG_EXTENSION_START,getRank());
					m_outbox.push_back(aMessage);
				}
				VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(sizeof(VERTEX_TYPE)*1);
				message[0]=m_EXTENSION_contigs[m_SEEDING_i][m_EXTENSION_currentPosition];
				m_EXTENSION_currentPosition++;
				Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,MASTER_RANK,TAG_EXTENSION_DATA,getRank());
				m_outbox.push_back(aMessage);
				if(m_EXTENSION_currentPosition==(int)m_EXTENSION_contigs[m_SEEDING_i].size()){
					Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,MASTER_RANK,TAG_EXTENSION_END,getRank());
					m_outbox.push_back(aMessage);
					m_SEEDING_i++;
					m_EXTENSION_currentPosition=0;
				}
			}
		}
	}else if(m_mode==MODE_FUSION){
		makeFusions();
	}

	if(m_COPY_ranks==getSize()){
		m_COPY_ranks=-1;
		cout<<"Rank "<<getRank()<<" directions copied."<<endl;
		m_master_mode=MODE_ASSEMBLE_GRAPH;
	}

	if(m_EXTENSION_numberOfRanksDone==getSize()){

		#ifndef SHOW_PROGRESS
		cout<<"\r"<<"Computing fusions"<<endl;
		#endif
		// ask one at once to do the fusion
		// because otherwise it may lead to hanging of the program for unknown reasons
		m_EXTENSION_numberOfRanksDone=-1;
		m_master_mode=MODE_DO_NOTHING;
		m_FUSION_numberOfRanksDone=0;
		for(int i=0;i<(int)getSize();i++){// start fusion.
			Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,i,TAG_START_FUSION,getRank());
			m_outbox.push_back(aMessage);
		}
		m_fusionStarted=true;
	}

	if(m_master_mode==MODE_ASSEMBLE_GRAPH){
		// ask ranks to send their extensions.
		if(!m_EXTENSION_currentRankIsSet){
			m_EXTENSION_currentRankIsSet=true;
			m_EXTENSION_currentRankIsStarted=false;
			m_EXTENSION_rank++;
		}
		if(m_EXTENSION_rank==getSize()){
			m_master_mode=MODE_DO_NOTHING;
			cout<<"Rank "<<getRank()<<" contigs computed."<<endl;
			killRanks();
		}else if(!m_EXTENSION_currentRankIsStarted){
			m_EXTENSION_currentRankIsStarted=true;
			Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,m_EXTENSION_rank,TAG_ASSEMBLE_WAVES,getRank());
			m_outbox.push_back(aMessage);
			m_EXTENSION_currentRankIsDone=false;
		}else if(m_EXTENSION_currentRankIsDone){
			m_EXTENSION_currentRankIsSet=false;
		}
	}

	if(m_FUSION_numberOfRanksDone==getSize() and !m_isFinalFusion){
		#ifdef SHOW_PROGRESS
		cout<<"Rank "<<getRank()<<": fusion is done."<<endl;
		#else
		cout<<"\r"<<"Finishing fusions"<<endl;
		#endif
		m_FUSION_numberOfRanksDone=-1;

		m_reductionOccured=true;
		m_cycleStarted=false;
		m_cycleNumber=0;
	}

	if(m_reductionOccured){
		// the finishing is
		//
		//  * a clear cycle
		//  * a distribute cycle
		//  * a finish cycle
		//  * a clear cycle
		//  * a distribute cycle
		//  * a fusion cycle

		if(!m_cycleStarted){
			#ifdef SHOW_PROGRESS
			cout<<"1 TAG_CLEAR_DIRECTIONS"<<endl;
			#endif
			m_nextReductionOccured=false;
			m_cycleStarted=true;
			m_isFinalFusion=false;
			for(int i=0;i<getSize();i++){
				Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,i,TAG_CLEAR_DIRECTIONS,getRank());
				m_outbox.push_back(aMessage);
			}
	
			m_CLEAR_n=0;
		}else if(m_CLEAR_n==getSize() and !m_isFinalFusion){
			#ifdef SHOW_PROGRESS
			cout<<"2 TAG_DISTRIBUTE_FUSIONS"<<endl;
			#endif
			m_CLEAR_n=-1;

			for(int i=0;i<getSize();i++){
				Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,i,TAG_DISTRIBUTE_FUSIONS,getRank());
				m_outbox.push_back(aMessage);
			}
			m_DISTRIBUTE_n=0;
		}else if(m_DISTRIBUTE_n==getSize() and !m_isFinalFusion){
			#ifdef SHOW_PROGRESS
			cout<<"3 TAG_FINISH_FUSIONS"<<endl;
			#endif
			m_DISTRIBUTE_n=-1;
			m_isFinalFusion=true;
			for(int i=0;i<getSize();i++){
				Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,i,TAG_FINISH_FUSIONS,getRank());
				m_outbox.push_back(aMessage);
			}
			m_FINISH_n=0;
		}else if(m_FINISH_n==getSize() and m_isFinalFusion){
			#ifdef SHOW_PROGRESS
			cout<<"4 TAG_CLEAR_DIRECTIONS"<<endl;
			#endif
			for(int i=0;i<getSize();i++){
				Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,i,TAG_CLEAR_DIRECTIONS,getRank());
				m_outbox.push_back(aMessage);
			}
			m_FINISH_n=-1;
			m_CLEAR_n=0;
		}else if(m_CLEAR_n==getSize() and m_isFinalFusion){
			m_CLEAR_n=-1;
			#ifdef SHOW_PROGRESS
			cout<<"5 TAG_DISTRIBUTE_FUSIONS"<<endl;
			#endif

			for(int i=0;i<getSize();i++){
				Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,i,TAG_DISTRIBUTE_FUSIONS,getRank());
				m_outbox.push_back(aMessage);
			}
			m_DISTRIBUTE_n=0;

		}else if(m_DISTRIBUTE_n==getSize() and m_isFinalFusion){
			#ifdef SHOW_PROGRESS
			cout<<"6 TAG_START_FUSION"<<endl;
			#endif
			m_FUSION_numberOfRanksDone=0;
			m_master_mode=MODE_DO_NOTHING;
			m_DISTRIBUTE_n=-1;
			for(int i=0;i<(int)getSize();i++){// start fusion.
				Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,i,TAG_START_FUSION,getRank());
				m_outbox.push_back(aMessage);
			}
			
		}else if(m_FUSION_numberOfRanksDone==getSize() and m_isFinalFusion){
			m_reductionOccured=m_nextReductionOccured;
			m_FUSION_numberOfRanksDone=-1;
			if(!m_reductionOccured or m_cycleNumber ==5){ // cycling is in development!
				cout<<"\r"<<"Collecting fusions"<<endl;
				m_master_mode=MODE_ASK_EXTENSIONS;
				m_EXTENSION_currentRankIsSet=false;
				m_EXTENSION_rank=-1;
			}else{
				// we continue now!
				m_cycleStarted=false;
				m_cycleNumber++;
			}
		}
	}

	if(m_master_mode==MODE_ASK_EXTENSIONS){
		#ifndef SHOW_PROGRESS
		time_t tmp=time(NULL);
		if(tmp>m_lastTime){
			m_lastTime=tmp;
			showProgress();
		}
		#endif

		// ask ranks to send their extensions.
		if(!m_EXTENSION_currentRankIsSet){
			m_EXTENSION_currentRankIsSet=true;
			m_EXTENSION_currentRankIsStarted=false;
			m_EXTENSION_rank++;
		}
		if(m_EXTENSION_rank==getSize()){
			m_master_mode=MODE_DO_NOTHING;

			
			#ifdef DEBUG
			assert(m_allPaths.size()==m_identifiers.size());
			#endif
			ofstream f(m_parameters.getOutputFile().c_str());
			for(int i=0;i<(int)m_allPaths.size();i++){
				string contig=convertToString(&(m_allPaths[i]),m_wordSize);
				#ifdef DEBUG
				assert(i<(int)m_identifiers.size());
				#endif
				int id=m_identifiers[i];
				#ifdef DEBUG
				int theRank=id%MAX_NUMBER_OF_MPI_PROCESSES;
				assert(theRank<getSize());
				#endif
				f<<">contig-"<<id<<" "<<contig.length()<<" nucleotides"<<endl<<addLineBreaks(contig);
			}
			f.close();
			#ifdef SHOW_PROGRESS
			cout<<"Rank "<<getRank()<<" wrote "<<m_parameters.getOutputFile()<<endl;
			#else
			cout<<"\r"<<"Writing "<<m_parameters.getOutputFile()<<endl;
			#endif
			if(m_parameters.useAmos()){
				m_master_mode=MODE_AMOS;
				m_SEEDING_i=0;
				m_mode_send_vertices_sequence_id_position=0;
				m_EXTENSION_reads_requested=false;
				cout<<"\rCompleting "<<m_parameters.getAmosFile()<<endl;
			}else{// we are done.
				killRanks();
			}
			
		}else if(!m_EXTENSION_currentRankIsStarted){
			m_EXTENSION_currentRankIsStarted=true;
			#ifdef SHOW_PROGRESS
			cout<<"Rank "<<getRank()<<" asks "<<m_EXTENSION_rank<<" for its extensions."<<endl;
			#endif
			Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,m_EXTENSION_rank,TAG_ASK_EXTENSION_DATA,getRank());
			m_outbox.push_back(aMessage);
			m_EXTENSION_currentRankIsDone=false;
		}else if(m_EXTENSION_currentRankIsDone){
			m_EXTENSION_currentRankIsSet=false;
		}
	}else if(m_master_mode==MODE_AMOS){
		// in development.
		/*
 		* use m_allPaths and m_identifiers
 		*
 		* iterators: m_SEEDING_i: for the current contig
 		*            m_mode_send_vertices_sequence_id_position: for the current position in the current contig.
 		*/
		if(m_SEEDING_i==(int)m_allPaths.size()){// all contigs are processed
			killRanks();
			m_master_mode=MODE_DO_NOTHING;
			fclose(m_bubbleData->m_amos);
		}else if(m_mode_send_vertices_sequence_id_position==(int)m_allPaths[m_SEEDING_i].size()){// iterate over the next one
			m_SEEDING_i++;
			m_mode_send_vertices_sequence_id_position=0;
			m_EXTENSION_reads_requested=false;
			
			FILE*fp=m_bubbleData->m_amos;
			fprintf(fp,"}\n");
		}else{
			if(!m_EXTENSION_reads_requested){
				if(m_mode_send_vertices_sequence_id_position==0){
					FILE*fp=m_bubbleData->m_amos;
					string seq=convertToString(&(m_allPaths[m_SEEDING_i]),m_wordSize);
					char*qlt=(char*)__Malloc(seq.length()+1);
					strcpy(qlt,seq.c_str());
					for(int i=0;i<(int)strlen(qlt);i++)
						qlt[i]='D';
					fprintf(fp,"{CTG\niid:%i\neid:contig-%i\ncom:\nAssembly engine: %s %s\n.\nseq:\n%s\n.\nqlt:\n%s\n.\n",
						m_SEEDING_i+1,
						m_identifiers[m_SEEDING_i],
						m_parameters.getEngineName().c_str(),
						m_parameters.getVersion().c_str(),
						seq.c_str(),
						qlt
						);
					__Free(qlt);
				}

				m_EXTENSION_reads_requested=true;
				m_EXTENSION_reads_received=false;
				VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
				message[0]=m_allPaths[m_SEEDING_i][m_mode_send_vertices_sequence_id_position];
				Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(message[0]),TAG_REQUEST_READS,getRank());
				m_outbox.push_back(aMessage);

				// iterator on reads
				m_FUSION_path_id=0;
				m_EXTENSION_readLength_requested=false;
			}else if(m_EXTENSION_reads_received){
				if(m_FUSION_path_id<(int)m_EXTENSION_receivedReads.size()){
					int readRank=m_EXTENSION_receivedReads[m_FUSION_path_id].getRank();
					char strand=m_EXTENSION_receivedReads[m_FUSION_path_id].getStrand();
					int idOnRank=m_EXTENSION_receivedReads[m_FUSION_path_id].getReadIndex();
					if(!m_EXTENSION_readLength_requested){
						m_EXTENSION_readLength_requested=true;
						m_EXTENSION_readLength_received=false;
						VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
						message[0]=idOnRank;
						Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,readRank,TAG_ASK_READ_LENGTH,getRank());
						m_outbox.push_back(aMessage);
					}else if(m_EXTENSION_readLength_received){
						int globalIdentifier=idOnRank*getSize()+readRank;
						FILE*fp=m_bubbleData->m_amos;
						int start=0;
						int readLength=m_EXTENSION_receivedLength;
						int theEnd=readLength-1;
						int offset=m_mode_send_vertices_sequence_id_position;
						if(strand=='R'){
							int t=start;
							start=theEnd;
							theEnd=t;
							offset++;
						}
						fprintf(fp,"{TLE\nsrc:%i\noff:%i\nclr:%i,%i\n}\n",globalIdentifier+1,offset,
							start,theEnd);
			
						// increment to get the next read.
						m_FUSION_path_id++;
						m_EXTENSION_readLength_requested=false;
					}
				}else{
					// continue.
					m_mode_send_vertices_sequence_id_position++;
					m_EXTENSION_reads_requested=false;
				}
			}

		}
	}

	if(m_mode_EXTENSION){
		extendSeeds();
	}
}

/*
 * check if (m_SEEDING_currentRank,m_SEEDING_currentPointer) has
 * 1 ingoing edge and 1 outgoing edge
 *
 * before entering the first call, m_SEEDING_testInitiated and m_SEEDING_1_1_test_done must be false
 *
 * outputs:
 *
 *  m_SEEDING_1_1_test_done
 *  m_SEEDING_currentChildVertex
 *  m_SEEDING_currentChildRank
 *  m_SEEDING_currentChildPointer
 *  m_SEEDING_currentParentRank
 *  m_SEEDING_currentParentPointer
 *
 *
 *  internals:
 *
 *  m_SEEDING_InedgesRequested
 *  m_SEEDING_InedgesReceived
 *  m_SEEDING_Inedge
 *  m_SEEDING_edgesRequested
 *  m_SEEDING_edgesReceived
 */
void Machine::do_1_1_test(){
	if(m_SEEDING_1_1_test_done){
		return;
	}else if(!m_SEEDING_testInitiated){
		m_SEEDING_testInitiated=true;
		m_SEEDING_ingoingEdgesDone=false;
		m_SEEDING_InedgesRequested=false;
	}else if(!m_SEEDING_ingoingEdgesDone){
		if(!m_SEEDING_InedgesRequested){
			VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
			message[0]=(VERTEX_TYPE)m_SEEDING_currentVertex;
			Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(m_SEEDING_currentVertex),TAG_REQUEST_VERTEX_INGOING_EDGES,getRank());
			m_outbox.push_back(aMessage);
			m_SEEDING_numberOfIngoingEdges=0;
			m_SEEDING_numberOfIngoingEdgesWithSeedCoverage=0;
			m_SEEDING_vertexCoverageRequested=false;
			m_SEEDING_InedgesReceived=false;
			m_SEEDING_InedgesRequested=true;
			m_SEEDING_ingoingEdgeIndex=0;
		}else if(m_SEEDING_InedgesReceived){
			if(m_SEEDING_ingoingEdgeIndex<(int)m_SEEDING_receivedIngoingEdges.size()){
				if(!m_SEEDING_vertexCoverageRequested){
					VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
					message[0]=(VERTEX_TYPE)m_SEEDING_receivedIngoingEdges[m_SEEDING_ingoingEdgeIndex];
					Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(message[0]),TAG_REQUEST_VERTEX_COVERAGE,getRank());
					m_outbox.push_back(aMessage);
					m_SEEDING_vertexCoverageRequested=true;
					m_SEEDING_vertexCoverageReceived=false;
					m_SEEDING_receivedVertexCoverage=-1;
				}else if(m_SEEDING_vertexCoverageReceived){
					if(m_SEEDING_receivedIngoingEdges.size()==1){//there is only one anyway
						m_SEEDING_currentParentVertex=m_SEEDING_receivedIngoingEdges[m_SEEDING_ingoingEdgeIndex];
					}
					if(m_SEEDING_receivedVertexCoverage>=m_seedCoverage){
						m_SEEDING_currentParentVertex=m_SEEDING_receivedIngoingEdges[m_SEEDING_ingoingEdgeIndex];
						m_SEEDING_numberOfIngoingEdgesWithSeedCoverage++;
					}
					m_SEEDING_ingoingEdgeIndex++;
					m_SEEDING_numberOfIngoingEdges++;
					m_SEEDING_vertexCoverageRequested=false;
				}
			}else{// done analyzing ingoing edges.
				m_SEEDING_ingoingEdgesDone=true;
				m_SEEDING_outgoingEdgesDone=false;
				m_SEEDING_edgesRequested=false;
			}
		}
	}else if(!m_SEEDING_outgoingEdgesDone){
		if(!m_SEEDING_edgesRequested){
			VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
			message[0]=(VERTEX_TYPE)m_SEEDING_currentVertex;
			Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(m_SEEDING_currentVertex),TAG_REQUEST_VERTEX_OUTGOING_EDGES,getRank());
			m_outbox.push_back(aMessage);
			m_SEEDING_edgesRequested=true;
			m_SEEDING_numberOfOutgoingEdges=0;
			m_SEEDING_numberOfOutgoingEdgesWithSeedCoverage=0;
			m_SEEDING_vertexCoverageRequested=false;
			m_SEEDING_edgesReceived=false;
			m_SEEDING_outgoingEdgeIndex=0;
		}else if(m_SEEDING_edgesReceived){
			if(m_SEEDING_outgoingEdgeIndex<(int)m_SEEDING_receivedOutgoingEdges.size()){
				// TODO: don't check the coverage if there is only one
				if(!m_SEEDING_vertexCoverageRequested){
					VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
					message[0]=(VERTEX_TYPE)m_SEEDING_receivedOutgoingEdges[m_SEEDING_outgoingEdgeIndex];
					Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(message[0]),TAG_REQUEST_VERTEX_COVERAGE,getRank());
					m_outbox.push_back(aMessage);
					m_SEEDING_vertexCoverageRequested=true;
					m_SEEDING_vertexCoverageReceived=false;
					m_SEEDING_receivedVertexCoverage=-1;
				}else if(m_SEEDING_vertexCoverageReceived){
					if(m_SEEDING_receivedOutgoingEdges.size()==1){//there is only one anyway
						m_SEEDING_currentChildVertex=m_SEEDING_receivedOutgoingEdges[m_SEEDING_outgoingEdgeIndex];
					}
					if(m_SEEDING_receivedVertexCoverage>=m_seedCoverage){
						m_SEEDING_currentChildVertex=m_SEEDING_receivedOutgoingEdges[m_SEEDING_outgoingEdgeIndex];
						m_SEEDING_numberOfOutgoingEdgesWithSeedCoverage++;
					}
					m_SEEDING_outgoingEdgeIndex++;
					m_SEEDING_numberOfOutgoingEdges++;
					m_SEEDING_vertexCoverageRequested=false;
				}
			}else{// done analyzing ingoing edges.
				m_SEEDING_outgoingEdgesDone=true;
			}
		}


	}else{
		m_SEEDING_1_1_test_done=true;
		m_SEEDING_1_1_test_result=(m_SEEDING_numberOfIngoingEdgesWithSeedCoverage==1)and
			(m_SEEDING_numberOfOutgoingEdgesWithSeedCoverage==1);
	}
}

void Machine::killRanks(){
	for(int i=0;i<getSize();i++){
		Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,i,TAG_GOOD_JOB_SEE_YOU_SOON,getRank());
		m_outbox.push_back(aMessage);
	}
}

bool Machine::isMaster(){
	return getRank()==MASTER_RANK;
}



void Machine::extendSeeds(){
	if(m_SEEDING_seeds.size()==0){
		#ifdef SHOW_PROGRESS
		cout<<"Rank "<<getRank()<<": extending seeds "<<m_SEEDING_seeds.size()<<"/"<<m_SEEDING_seeds.size()<<" (DONE)"<<endl;
		#endif
		m_mode_EXTENSION=false;
		Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,MASTER_RANK,TAG_EXTENSION_IS_DONE,getRank());
		m_outbox.push_back(aMessage);
		return;
	}
	if(!m_EXTENSION_initiated){
		m_EXTENSION_initiated=true;
		m_EXTENSION_currentSeedIndex=0;
		m_EXTENSION_currentPosition=0;
		m_EXTENSION_currentSeed=m_SEEDING_seeds[m_EXTENSION_currentSeedIndex];
		m_SEEDING_currentVertex=m_EXTENSION_currentSeed[m_EXTENSION_currentPosition];
		m_EXTENSION_checkedIfCurrentVertexIsAssembled=false;
		m_EXTENSION_directVertexDone=false;
		m_EXTENSION_VertexAssembled_requested=false;
		m_EXTENSION_extension.clear();
		m_EXTENSION_complementedSeed=false;
		m_EXTENSION_reads_startingPositionOnContig.clear();
		m_EXTENSION_readsInRange.clear();
	}else if(m_EXTENSION_currentSeedIndex==(int)m_SEEDING_seeds.size()){
		#ifdef SHOW_PROGRESS
		cout<<"Rank "<<getRank()<<": extending seeds "<<m_SEEDING_seeds.size()<<"/"<<m_SEEDING_seeds.size()<<" (DONE)"<<endl;
		#endif
		m_mode_EXTENSION=false;
		
		// store the lengths.
		for(int i=0;i<(int)m_EXTENSION_identifiers.size();i++){
			int id=m_EXTENSION_identifiers[i];
			m_FUSION_identifier_map[id]=i;
		}

		Message aMessage(NULL,0,MPI_UNSIGNED_LONG_LONG,MASTER_RANK,TAG_EXTENSION_IS_DONE,getRank());
		m_outbox.push_back(aMessage);
		return;
	}

	

	// algorithms here.
	// if the current vertex is assembled or if its reverse complement is assembled, return
	// else, mark it as assembled, and mark its reverse complement as assembled too.
	// 	enumerate the available choices
	// 	if choices are included in the seed itself
	// 		choose it
	// 	else
	// 		use read paths or pairs of reads to resolve the repeat.
	
	if(!m_EXTENSION_checkedIfCurrentVertexIsAssembled){
		checkIfCurrentVertexIsAssembled();
	}else if(m_EXTENSION_vertexIsAssembledResult and m_EXTENSION_currentPosition==0 and m_EXTENSION_complementedSeed==false){
		m_EXTENSION_currentSeedIndex++;// skip the current one.
		m_EXTENSION_currentPosition=0;
		m_EXTENSION_checkedIfCurrentVertexIsAssembled=false;
		m_EXTENSION_directVertexDone=false;
		m_EXTENSION_VertexAssembled_requested=false;
		if(m_EXTENSION_currentSeedIndex<(int)m_SEEDING_seeds.size()){
			m_EXTENSION_currentSeed=m_SEEDING_seeds[m_EXTENSION_currentSeedIndex];
			m_SEEDING_currentVertex=m_EXTENSION_currentSeed[m_EXTENSION_currentPosition];
		}
		// TODO: check if the position !=0
		m_EXTENSION_complementedSeed=false;
		m_EXTENSION_extension.clear();
		m_EXTENSION_reads_startingPositionOnContig.clear();
		m_EXTENSION_readsInRange.clear();
	}else if(!m_EXTENSION_markedCurrentVertexAsAssembled){
		markCurrentVertexAsAssembled();
	}else if(!m_EXTENSION_enumerateChoices){
		enumerateChoices();
	}else if(!m_EXTENSION_choose){
		doChoice();
	}

}

void Machine::enumerateChoices(){
	if(!m_SEEDING_edgesRequested){
		m_EXTENSION_coverages.clear();
		m_SEEDING_edgesReceived=false;
		m_SEEDING_edgesRequested=true;
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
		message[0]=(VERTEX_TYPE)m_SEEDING_currentVertex;
		Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(m_SEEDING_currentVertex),TAG_REQUEST_VERTEX_OUTGOING_EDGES,getRank());
		m_outbox.push_back(aMessage);
		m_EXTENSION_currentPosition++;
		m_SEEDING_vertexCoverageRequested=false;
		m_SEEDING_outgoingEdgeIndex=0;
	}else if(m_SEEDING_edgesReceived){
		if(m_SEEDING_outgoingEdgeIndex<(int)m_SEEDING_receivedOutgoingEdges.size()){
			// get the coverage of these.
			if(!m_SEEDING_vertexCoverageRequested){
				VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
				message[0]=(VERTEX_TYPE)m_SEEDING_receivedOutgoingEdges[m_SEEDING_outgoingEdgeIndex];
				Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(message[0]),TAG_REQUEST_VERTEX_COVERAGE,getRank());
				m_outbox.push_back(aMessage);
				m_SEEDING_vertexCoverageRequested=true;
				m_SEEDING_vertexCoverageReceived=false;
				m_SEEDING_receivedVertexCoverage=-1;
			}else if(m_SEEDING_vertexCoverageReceived){
				m_SEEDING_outgoingEdgeIndex++;
				m_SEEDING_vertexCoverageRequested=false;
				m_EXTENSION_coverages.push_back(m_SEEDING_receivedVertexCoverage);
			}
		}else{
			m_EXTENSION_enumerateChoices=true;
			m_EXTENSION_choose=false;
			m_EXTENSION_singleEndResolution=false;
			m_EXTENSION_readIterator=m_EXTENSION_readsInRange.begin();
			m_EXTENSION_readsOutOfRange.clear();
			m_EXTENSION_readLength_done=false;
			m_EXTENSION_readLength_requested=false;
			m_EXTENSION_readPositionsForVertices.clear();
			m_EXTENSION_pairedReadPositionsForVertices.clear();
			m_enumerateChoices_outgoingEdges=m_SEEDING_receivedOutgoingEdges;
		}
	}
}

int Machine::proceedWithCoverages(int a,int b){
	vector<int> counts2;
	vector<int> counts5;
	for(int i=0;i<(int)m_EXTENSION_coverages.size();i++){
		int j2=0;
		int j5=0;
		if(m_EXTENSION_readPositionsForVertices.count(i)>0){
			for(int k=0;k<(int)m_EXTENSION_readPositionsForVertices[i].size();k++){
				int distanceFromOrigin=m_EXTENSION_readPositionsForVertices[i][k];
				if(distanceFromOrigin>=2){
					j2++;
				}
				if(distanceFromOrigin>=5){
					j5++;
				}
			}
		}
		counts2.push_back(j2);
		counts5.push_back(j5);
	}

	for(int i=0;i<(int)m_EXTENSION_coverages.size();i++){
		bool isBetter=true;
		int coverageI=m_EXTENSION_coverages[i];
		int singleReadsI=m_EXTENSION_readPositionsForVertices[i].size();
		if(counts2[i]==0)
			continue;

		// in less than 10% of the coverage is supported by displayed reads, abort it...
		if(singleReadsI*10 < coverageI){
			continue;
		}

		for(int j=0;j<(int)m_EXTENSION_coverages.size();j++){
			if(i==j)
				continue;
			//int coverageJ=m_EXTENSION_coverages[j];
			int singleReadsJ=m_EXTENSION_readPositionsForVertices[j].size();
			if(!(singleReadsJ<=a and singleReadsI>=b)){
				isBetter=false;
				break;
			}
		}
		if(isBetter){
			#ifdef SHOW_PROGRESS
			cout<<"Choice #"<<i+1<<" wins, with "<<m_EXTENSION_readPositionsForVertices[i].size()<<" reads."<<endl;
			cout<<" in ranges: "<<m_EXTENSION_readsInRange.size()<<endl;
			#endif
			m_SEEDING_currentVertex=m_enumerateChoices_outgoingEdges[i];
			m_EXTENSION_choose=true;
			m_EXTENSION_checkedIfCurrentVertexIsAssembled=false;
			m_EXTENSION_directVertexDone=false;
			m_EXTENSION_VertexAssembled_requested=false;

			return i;
		}
	}
	return -1;
}

void Machine::doChoice(){
	// use seed information.
	#ifdef SHOW_PROGRESS
	if(m_EXTENSION_currentPosition==1)
		cout<<"Priming with seed length="<<m_EXTENSION_currentSeed.size()<<endl;
	#endif

	if(m_EXTENSION_currentPosition<(int)m_EXTENSION_currentSeed.size()){
		for(int i=0;i<(int)m_enumerateChoices_outgoingEdges.size();i++){
			if(m_enumerateChoices_outgoingEdges[i]==m_EXTENSION_currentSeed[m_EXTENSION_currentPosition]){
				m_SEEDING_currentVertex=m_enumerateChoices_outgoingEdges[i];
				m_EXTENSION_choose=true;
				m_EXTENSION_checkedIfCurrentVertexIsAssembled=false;
				m_EXTENSION_directVertexDone=false;
				m_EXTENSION_VertexAssembled_requested=false;
				return;
			}
		}
	// else, do a paired-end or single-end lookup if reads are in range.
	}else{

/*
 *
 *                         min                          seed                       peak
 *             min/2       
 *                                      2min
 *   A         ==============
 *   B                      =============================
 *   C                      =============
 */

		if(!m_EXTENSION_singleEndResolution and m_EXTENSION_readsInRange.size()>0){
			// try to use single-end reads to resolve the repeat.
			// for each read in range, ask them their vertex at position (CurrentPositionOnContig-StartPositionOfReadOnContig)
			// and cumulate the results in
			// m_EXTENSION_readPositions, which is a map<int,vector<int> > if one of the vertices match
			if(m_EXTENSION_readIterator!=m_EXTENSION_readsInRange.end()){
				if(!m_EXTENSION_readLength_done){
					if(!m_EXTENSION_readLength_requested){
						m_EXTENSION_readLength_requested=true;
						m_EXTENSION_readLength_received=false;

						ReadAnnotation annotation=*m_EXTENSION_readIterator;
						VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
						message[0]=annotation.getReadIndex();
						Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,annotation.getRank(),TAG_ASK_READ_LENGTH,getRank());
						m_outbox.push_back(aMessage);
					}else if(m_EXTENSION_readLength_received){
						ReadAnnotation annotation=*m_EXTENSION_readIterator;

						int startPosition=m_EXTENSION_reads_startingPositionOnContig[annotation.getUniqueId()];
						int distance=m_EXTENSION_extension.size()-startPosition;
						if(distance>(m_EXTENSION_receivedLength-m_wordSize)){
							// the read is now out-of-range.
							m_EXTENSION_readsOutOfRange.push_back(annotation);
							m_EXTENSION_readLength_done=false;
							m_EXTENSION_readLength_requested=false;
							m_EXTENSION_readIterator++;
						}else{
							m_EXTENSION_readLength_done=true;
							m_EXTENSION_read_vertex_requested=false;
						}
					}
				}else if(!m_EXTENSION_read_vertex_requested){
					m_EXTENSION_read_vertex_requested=true;
					ReadAnnotation annotation=*m_EXTENSION_readIterator;
					int startPosition=m_EXTENSION_reads_startingPositionOnContig[annotation.getUniqueId()];
					if(!(0<=startPosition and startPosition<(int)m_EXTENSION_extension.size())){
						cout<<"FATAL"<<endl;
						cout<<"The read started at "<<startPosition<<endl;
						cout<<"The extension has "<<m_EXTENSION_extension.size()<<" elements."<<endl;
						cout<<"The read has length="<<m_EXTENSION_receivedLength<<endl;
					}
					int distance=m_EXTENSION_extension.size()-startPosition;
					VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(3*sizeof(VERTEX_TYPE));
					message[0]=annotation.getReadIndex();
					message[1]=distance;
					message[2]=annotation.getStrand();
					Message aMessage(message,3,MPI_UNSIGNED_LONG_LONG,annotation.getRank(),TAG_ASK_READ_VERTEX_AT_POSITION,getRank());
					m_outbox.push_back(aMessage);
					m_EXTENSION_read_vertex_received=false;
					m_EXTENSION_edgeIterator=0;
					m_EXTENSION_hasPairedReadRequested=false;
				}else if(m_EXTENSION_read_vertex_received){
					ReadAnnotation annotation=*m_EXTENSION_readIterator;
					int startPosition=m_EXTENSION_reads_startingPositionOnContig[annotation.getUniqueId()];
					int distance=m_EXTENSION_extension.size()-startPosition;

					// process each edge separately.
					if(m_EXTENSION_edgeIterator<(int)m_enumerateChoices_outgoingEdges.size()){
						// got a match!
						if(m_EXTENSION_receivedReadVertex==m_enumerateChoices_outgoingEdges[m_EXTENSION_edgeIterator]){
							ReadAnnotation annotation=*m_EXTENSION_readIterator;
							// check if the current read has a paired read.
							if(!m_EXTENSION_hasPairedReadRequested){
								VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
								message[0]=annotation.getReadIndex();
								Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,annotation.getRank(),TAG_HAS_PAIRED_READ,getRank());
								m_outbox.push_back(aMessage);
								m_EXTENSION_hasPairedReadRequested=true;
								m_EXTENSION_hasPairedReadReceived=false;
								m_EXTENSION_pairedSequenceRequested=false;
							}else if(m_EXTENSION_hasPairedReadReceived){
								// vertex matches, but no paired end read found, at last.
								if(!m_EXTENSION_hasPairedReadAnswer){
									m_EXTENSION_readPositionsForVertices[m_EXTENSION_edgeIterator].push_back(distance);
									m_EXTENSION_edgeIterator++;
									m_EXTENSION_hasPairedReadRequested=false;
								}else{
									// get the paired end read.
									if(!m_EXTENSION_pairedSequenceRequested){
										m_EXTENSION_pairedSequenceRequested=true;
										VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
										message[0]=annotation.getReadIndex();
										Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,annotation.getRank(),TAG_GET_PAIRED_READ,getRank());
										m_outbox.push_back(aMessage);
										m_EXTENSION_pairedSequenceReceived=false;
									}else if(m_EXTENSION_pairedSequenceReceived){
										int expectedFragmentLength=m_EXTENSION_pairedRead.getAverageFragmentLength();
										int expectedDeviation=m_EXTENSION_pairedRead.getStandardDeviation();
										int rank=m_EXTENSION_pairedRead.getRank();
										int id=m_EXTENSION_pairedRead.getId();
										int uniqueReadIdentifier=id*MAX_NUMBER_OF_MPI_PROCESSES+rank;
			
										// it is mandatory for a read to start at 0. at position X on the path.
										if(m_EXTENSION_reads_startingPositionOnContig.count(uniqueReadIdentifier)>0){
											int startingPositionOnPath=m_EXTENSION_reads_startingPositionOnContig[uniqueReadIdentifier];
											int observedFragmentLength=(startPosition-startingPositionOnPath)+m_EXTENSION_receivedLength;
											if(expectedFragmentLength-expectedDeviation<=observedFragmentLength and
											observedFragmentLength <= expectedFragmentLength+expectedDeviation){
											// it matches!
												int theDistance=startPosition-startingPositionOnPath+distance;
												m_EXTENSION_pairedReadPositionsForVertices[m_EXTENSION_edgeIterator].push_back(theDistance);
											}

										}
										
										// add it anyway as a single-end match too!
										m_EXTENSION_readPositionsForVertices[m_EXTENSION_edgeIterator].push_back(distance);
										m_EXTENSION_edgeIterator++;
										m_EXTENSION_hasPairedReadRequested=false;
									}
								}
							}else{
							}
						}else{// no match, too bad.
							m_EXTENSION_edgeIterator++;
							m_EXTENSION_hasPairedReadRequested=false;
						}
					}else{
						m_EXTENSION_readLength_done=false;
						m_EXTENSION_readLength_requested=false;
						m_EXTENSION_readIterator++;
					}
				}
			}else{
				for(int i=0;i<(int)m_EXTENSION_readsOutOfRange.size();i++){
					m_EXTENSION_readsInRange.erase(m_EXTENSION_readsOutOfRange[i]);
				}
				m_EXTENSION_readsOutOfRange.clear();
				m_EXTENSION_singleEndResolution=true;
				#ifdef SHOW_PROGRESS
				if(m_enumerateChoices_outgoingEdges.size()>1){
					cout<<endl;
					cout<<"*****************************************"<<endl;
					cout<<"CurrentVertex="<<idToWord(m_SEEDING_currentVertex,m_wordSize)<<" @"<<m_EXTENSION_extension.size()<<endl;
					cout<<" # ReadsInRange: "<<m_EXTENSION_readsInRange.size()<<endl;
					cout<<m_enumerateChoices_outgoingEdges.size()<<" arcs."<<endl;
					for(int i=0;i<(int)m_enumerateChoices_outgoingEdges.size();i++){
						string vertex=idToWord(m_enumerateChoices_outgoingEdges[i],m_wordSize);
						cout<<endl;
						cout<<"Choice #"<<i+1<<endl;
						cout<<"Vertex: "<<vertex<<endl;
						cout<<"Coverage="<<m_EXTENSION_coverages[i]<<endl;
						cout<<"New letter: "<<vertex[m_wordSize-1]<<endl;
						cout<<"Single-end reads:"<<endl;
						for(int j=0;j<(int)m_EXTENSION_readPositionsForVertices[i].size();j++){
							cout<<" "<<m_EXTENSION_readPositionsForVertices[i][j];
						}
						cout<<endl;
						cout<<"Paired-end reads:"<<endl;
						for(int j=0;j<(int)m_EXTENSION_pairedReadPositionsForVertices[i].size();j++){
							cout<<" "<<m_EXTENSION_pairedReadPositionsForVertices[i][j];
						}
						cout<<endl;
					}
				}
				#endif

				// try to use the coverage to choose.
				int i=proceedWithCoverages(m_minimumCoverage/2,m_minimumCoverage);
				if(i>=0){
					return;
				}
				i=proceedWithCoverages(m_minimumCoverage,2*m_minimumCoverage);
				if(i>=0)
					return;


				// paired-end resolution of repeats.
				map<int,int> theMaxsPaired;
				map<int,int> theSumsPaired;
				map<int,int> theNumbersPaired;
				for(int i=0;i<(int)m_EXTENSION_pairedReadPositionsForVertices.size();i++){
					int max=-1;
					for(int j=0;j<(int)m_EXTENSION_pairedReadPositionsForVertices[i].size();j++){
						int offset=m_EXTENSION_pairedReadPositionsForVertices[i][j];
						theSumsPaired[i]+=offset;
						if(offset>max){
							max=offset;
						}
					}
					theNumbersPaired[i]=m_EXTENSION_pairedReadPositionsForVertices[i].size();
					theMaxsPaired[i]=max;
				}
				// single-end resolution of repeats.
				map<int,int> theMaxs;
				map<int,int> theSums;
				map<int,int> theNumbers;
				for(int i=0;i<(int)m_EXTENSION_readPositionsForVertices.size();i++){
					int max=-1;
					for(int j=0;j<(int)m_EXTENSION_readPositionsForVertices[i].size();j++){
						int offset=m_EXTENSION_readPositionsForVertices[i][j];

						theSums[i]+=offset;
						if(offset>max){
							max=offset;
						}
					}
					theNumbers[i]=m_EXTENSION_readPositionsForVertices[i].size();
					theMaxs[i]=max;
				}


				for(int i=0;i<(int)m_EXTENSION_pairedReadPositionsForVertices.size();i++){
					bool winner=true;
					if(m_EXTENSION_coverages[i]<_MINIMUM_COVERAGE)
						continue;
					if(theNumbers[i]==0 or theNumbersPaired[i]==0)
						continue;
					for(int j=0;j<(int)m_EXTENSION_pairedReadPositionsForVertices.size();j++){
						if(i==j)
							continue;
						if(m_EXTENSION_coverages[j]<_MINIMUM_COVERAGE)
							continue;
						if((theMaxsPaired[i] <= 3*theMaxsPaired[j]) or
					 (theSumsPaired[i] <= 3*theSumsPaired[j]) or
					 (theNumbersPaired[i] <= 3*theNumbersPaired[j]) 
){
							winner=false;
							break;
						}
				
						// if the winner does not have too much coverage.
						if(m_EXTENSION_coverages[i]<m_minimumCoverage and 
					theNumbers[i] < theNumbers[j]){// make sure that it also has more single-end reads
							winner=false;
							break;
						}
					}
					if(winner==true){
						#ifdef SHOW_PROGRESS
						if(m_enumerateChoices_outgoingEdges.size()>1){
							cout<<"Choice "<<i+1<<" wins with paired-end reads."<<endl;
						}
						#endif
						m_SEEDING_currentVertex=m_enumerateChoices_outgoingEdges[i];
						m_EXTENSION_choose=true;
						m_EXTENSION_checkedIfCurrentVertexIsAssembled=false;
						m_EXTENSION_directVertexDone=false;
						m_EXTENSION_VertexAssembled_requested=false;
						return;
					}
				}

				for(int i=0;i<(int)m_EXTENSION_readPositionsForVertices.size();i++){
					bool winner=true;
					if(theMaxs[i]<5)
						winner=false;

					if(m_EXTENSION_coverages[i]<_MINIMUM_COVERAGE)
						continue;
					if(theNumbers[i]==0)
						continue;
					for(int j=0;j<(int)m_EXTENSION_readPositionsForVertices.size();j++){
						if(i==j)
							continue;

						if(m_EXTENSION_coverages[j]<_MINIMUM_COVERAGE)
							continue;
						if(	(theMaxs[i] <= 3*theMaxs[j]) 
							or (theSums[i] <= 3*theSums[j]) 
							or (theNumbers[i] <= 3*theNumbers[j])
							){
							winner=false;
							break;
						}
						if(m_EXTENSION_coverages[i]<m_minimumCoverage/4 and m_EXTENSION_coverages[j]>2*m_minimumCoverage){
							winner=false;
							break;
						}
					}
					if(winner==true){
						#ifdef SHOW_PROGRESS
						if(m_enumerateChoices_outgoingEdges.size()>1){
							cout<<"Choice "<<i+1<<" wins with single-end reads."<<endl;
						}
						#endif
						m_SEEDING_currentVertex=m_enumerateChoices_outgoingEdges[i];
						m_EXTENSION_choose=true;
						m_EXTENSION_checkedIfCurrentVertexIsAssembled=false;
						m_EXTENSION_directVertexDone=false;
						m_EXTENSION_VertexAssembled_requested=false;
						return;
					}
				}

				m_doChoice_tips_Detected=false;
				m_dfsData->m_doChoice_tips_Initiated=false;
				#ifdef SHOW_PROGRESS
				cout<<"Checking tips."<<endl;
				#endif
			}
			return;
		}else if(!m_doChoice_tips_Detected and m_EXTENSION_readsInRange.size()>0){
 			//for each entries in m_enumerateChoices_outgoingEdges, do a dfs of max depth 40.
			//if the reached depth is 40, it is not a tip, otherwise, it is.
			int maxDepth=MAX_DEPTH;
			if(!m_dfsData->m_doChoice_tips_Initiated){
				m_dfsData->m_doChoice_tips_i=0;
				m_dfsData->m_doChoice_tips_newEdges.clear();
				m_dfsData->m_doChoice_tips_dfs_initiated=false;
				m_dfsData->m_doChoice_tips_dfs_done=false;
				m_dfsData->m_doChoice_tips_Initiated=true;
				m_bubbleData->m_BUBBLE_visitedVertices.clear();
				m_bubbleData->m_visitedVertices.clear();
				m_bubbleData->m_BUBBLE_visitedVerticesDepths.clear();
				m_bubbleData->m_coverages.clear();
			}

			if(m_dfsData->m_doChoice_tips_i<(int)m_enumerateChoices_outgoingEdges.size()){
				if(!m_dfsData->m_doChoice_tips_dfs_done){
					depthFirstSearch(m_SEEDING_currentVertex,m_enumerateChoices_outgoingEdges[m_dfsData->m_doChoice_tips_i],maxDepth);
				}else{
					#ifdef SHOW_PROGRESS
					cout<<"Choice #"<<m_dfsData->m_doChoice_tips_i+1<<" : visited "<<m_dfsData->m_depthFirstSearchVisitedVertices.size()<<", max depth is "<<m_dfsData->m_depthFirstSearch_maxDepth<<endl;
					#endif
					// keep the edge if it is not a tip.
					if(m_dfsData->m_depthFirstSearch_maxDepth>=TIP_LIMIT){

						// just don't try that strange graph place for now.
						if(m_dfsData->m_depthFirstSearchVisitedVertices.size()==MAX_VERTICES_TO_VISIT){
							/*
							m_doChoice_tips_Detected=true;
							m_bubbleData->m_doChoice_bubbles_Detected=true;
							return;
							*/
						}
						m_dfsData->m_doChoice_tips_newEdges.push_back(m_dfsData->m_doChoice_tips_i);
						m_bubbleData->m_visitedVertices.push_back(m_dfsData->m_depthFirstSearchVisitedVertices);
						// store visited vertices for bubble detection purposes.
						m_bubbleData->m_BUBBLE_visitedVertices.push_back(m_dfsData->m_depthFirstSearchVisitedVertices_vector);
						m_bubbleData->m_coverages.push_back(m_dfsData->m_coverages);
						m_bubbleData->m_BUBBLE_visitedVerticesDepths.push_back(m_dfsData->m_depthFirstSearchVisitedVertices_depths);
					}else{
						#ifdef SHOW_PROGRESS
						cout<<"We have a tip "<<m_dfsData->m_depthFirstSearch_maxDepth<<" LIMIT="<<TIP_LIMIT<<"."<<endl;
						#endif
					}
					m_dfsData->m_doChoice_tips_i++;
					m_dfsData->m_doChoice_tips_dfs_initiated=false;
					m_dfsData->m_doChoice_tips_dfs_done=false;
				}
			}else{
				#ifdef SHOW_PROGRESS
				cout<<m_dfsData->m_doChoice_tips_newEdges.size()<<" new arcs."<<endl;
				#endif
				// we have a winner with tips investigation.
				if(m_dfsData->m_doChoice_tips_newEdges.size()==1 and m_EXTENSION_readsInRange.size()>0 
		and m_EXTENSION_readPositionsForVertices[m_dfsData->m_doChoice_tips_newEdges[0]].size()>0
){
					int readsInFavorOfThis=m_EXTENSION_readPositionsForVertices[m_dfsData->m_doChoice_tips_newEdges[0]].size();
					int coverageAtTheVertexLocation=m_EXTENSION_coverages[m_dfsData->m_doChoice_tips_newEdges[0]];

					// reads are not supportive of this.
					if(readsInFavorOfThis*10<coverageAtTheVertexLocation){
						// no luck..., yet.
						m_doChoice_tips_Detected=true;
						m_bubbleData->m_doChoice_bubbles_Detected=false;
						m_bubbleData->m_doChoice_bubbles_Initiated=false;
						return;
					}

					m_SEEDING_currentVertex=m_enumerateChoices_outgoingEdges[m_dfsData->m_doChoice_tips_newEdges[0]];
					#ifdef SHOW_PROGRESS
					cout<<"We have a win after tip elimination: "<<idToWord(m_SEEDING_currentVertex,m_wordSize)<<endl;
					#endif
					m_EXTENSION_choose=true;
					m_EXTENSION_checkedIfCurrentVertexIsAssembled=false;
					m_EXTENSION_directVertexDone=false;
					m_EXTENSION_VertexAssembled_requested=false;
					return;
				}else{
					// no luck..., yet.
					m_doChoice_tips_Detected=true;
					m_bubbleData->m_doChoice_bubbles_Detected=false;
					m_bubbleData->m_doChoice_bubbles_Initiated=false;
				}
			}
			return;
		// bubbles detection aims polymorphisms and homopolymers stretches.
		}else if(!m_bubbleData->m_doChoice_bubbles_Detected and m_EXTENSION_readsInRange.size()>0){
			BubbleTool tool;
			bool isGenuineBubble=tool.isGenuineBubble(m_SEEDING_currentVertex,&m_bubbleData->m_BUBBLE_visitedVertices);

			// support indels of 1 as well as mismatch polymorphisms.
			if(isGenuineBubble){
				cout<<"Forcing next choice "<<endl;
				m_SEEDING_currentVertex=m_enumerateChoices_outgoingEdges[m_dfsData->m_doChoice_tips_newEdges[0]];
				m_EXTENSION_choose=true;
				m_EXTENSION_checkedIfCurrentVertexIsAssembled=false;
				m_EXTENSION_directVertexDone=false;
				m_EXTENSION_VertexAssembled_requested=false;
			}
			m_bubbleData->m_doChoice_bubbles_Detected=true;
			return;
		}

		// no choice possible...
		// do it for the lulz
		if(!m_EXTENSION_complementedSeed){
			#ifdef SHOW_PROGRESS
			cout<<"Switching to reverse complement."<<endl;
			#endif
			m_EXTENSION_complementedSeed=true;
			vector<VERTEX_TYPE> complementedSeed;
			for(int i=m_EXTENSION_extension.size()-1;i>=0;i--){
				complementedSeed.push_back(complementVertex(m_EXTENSION_extension[i],m_wordSize,m_colorSpaceMode));
			}
			m_EXTENSION_currentPosition=0;
			m_EXTENSION_currentSeed=complementedSeed;
			m_EXTENSION_checkedIfCurrentVertexIsAssembled=false;
			m_SEEDING_currentVertex=m_EXTENSION_currentSeed[m_EXTENSION_currentPosition];
			m_EXTENSION_extension.clear();
			m_EXTENSION_usedReads.clear();
			m_EXTENSION_directVertexDone=false;
			m_EXTENSION_VertexAssembled_requested=false;
			m_EXTENSION_reads_startingPositionOnContig.clear();
			m_EXTENSION_readsInRange.clear();
		}else{
			if(m_EXTENSION_extension.size()>=100){
				#ifdef SHOW_PROGRESS
				cout<<"Rank "<<getRank()<<" stores an extension, "<<m_EXTENSION_extension.size()<<" vertices."<<endl;
				#endif
				m_EXTENSION_contigs.push_back(m_EXTENSION_extension);

				int id=m_EXTENSION_currentSeedIndex*MAX_NUMBER_OF_MPI_PROCESSES+getRank();
				m_EXTENSION_identifiers.push_back(id);
			}
			m_EXTENSION_currentSeedIndex++;
			m_EXTENSION_currentPosition=0;
			if(m_EXTENSION_currentSeedIndex<(int)m_SEEDING_seeds.size()){
				m_EXTENSION_currentSeed=m_SEEDING_seeds[m_EXTENSION_currentSeedIndex];
				m_SEEDING_currentVertex=m_EXTENSION_currentSeed[m_EXTENSION_currentPosition];
			}
			m_EXTENSION_checkedIfCurrentVertexIsAssembled=false;
			m_EXTENSION_extension.clear();
			m_EXTENSION_reads_startingPositionOnContig.clear();
			m_EXTENSION_readsInRange.clear();
			m_EXTENSION_usedReads.clear();
			m_EXTENSION_directVertexDone=false;
			m_EXTENSION_complementedSeed=false;
			m_EXTENSION_VertexAssembled_requested=false;
		}
	}
}

/*
 * do a depth first search with max depth of maxDepth;
 */
void Machine::depthFirstSearch(VERTEX_TYPE root,VERTEX_TYPE a,int maxDepth){
	if(!m_dfsData->m_doChoice_tips_dfs_initiated){
		m_dfsData->m_depthFirstSearchVisitedVertices.clear();
		m_dfsData->m_depthFirstSearchVisitedVertices_vector.clear();
		m_dfsData->m_depthFirstSearchVisitedVertices_depths.clear();
		while(m_dfsData->m_depthFirstSearchVerticesToVisit.size()>0)
			m_dfsData->m_depthFirstSearchVerticesToVisit.pop();
		while(m_dfsData->m_depthFirstSearchDepths.size()>0)
			m_dfsData->m_depthFirstSearchDepths.pop();

		m_dfsData->m_depthFirstSearchVerticesToVisit.push(a);
		m_dfsData->m_depthFirstSearchVisitedVertices.insert(a);
		m_dfsData->m_depthFirstSearchDepths.push(0);
		m_dfsData->m_depthFirstSearch_maxDepth=0;
		m_dfsData->m_doChoice_tips_dfs_initiated=true;
		m_dfsData->m_doChoice_tips_dfs_done=false;
		m_dfsData->m_coverages.clear();
		m_SEEDING_edgesRequested=false;
		m_SEEDING_vertexCoverageRequested=false;
		#ifdef SHOW_MINI_GRAPH
		cout<<"<MiniGraph>"<<endl;
		cout<<idToWord(root,m_wordSize)<<" -> "<<idToWord(a,m_wordSize)<<endl;
		#endif
	}
	if(m_dfsData->m_depthFirstSearchVerticesToVisit.size()>0){
		VERTEX_TYPE vertexToVisit=m_dfsData->m_depthFirstSearchVerticesToVisit.top();
		if(!m_SEEDING_vertexCoverageRequested){
			m_SEEDING_vertexCoverageRequested=true;
			m_SEEDING_vertexCoverageReceived=false;
			
			VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
			message[0]=vertexToVisit;
			Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(message[0]),TAG_REQUEST_VERTEX_COVERAGE,getRank());
			m_outbox.push_back(aMessage);
		}else if(m_SEEDING_vertexCoverageReceived){
			if(!m_SEEDING_edgesRequested){
				m_dfsData->m_coverages[vertexToVisit]=m_SEEDING_receivedVertexCoverage;
				if(m_SEEDING_receivedVertexCoverage>1){
					m_dfsData->m_depthFirstSearchVisitedVertices.insert(vertexToVisit);
				}else{
					// don't visit it.
					m_dfsData->m_depthFirstSearchVerticesToVisit.pop();
					m_dfsData->m_depthFirstSearchDepths.pop();
					m_SEEDING_edgesRequested=false;
					m_SEEDING_vertexCoverageRequested=false;
					return;
				}
				int theDepth=m_dfsData->m_depthFirstSearchDepths.top();
				if(m_dfsData->m_depthFirstSearchVisitedVertices.size()>=MAX_VERTICES_TO_VISIT){
					// quit this strange place.
	
					m_dfsData->m_doChoice_tips_dfs_done=true;
					#ifdef SHOW_PROGRESS
					cout<<"Exiting, I am lost. "<<m_dfsData->m_depthFirstSearchVisitedVertices.size()<<""<<endl;
					#endif
					return;
				}
				// too far away.
				if(theDepth> m_dfsData->m_depthFirstSearch_maxDepth){
					m_dfsData->m_depthFirstSearch_maxDepth=theDepth;
				}
			
				// visit the vertex, and ask next edges.
				VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
				message[0]=vertexToVisit;
				Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(vertexToVisit),TAG_REQUEST_VERTEX_OUTGOING_EDGES,getRank());
				m_outbox.push_back(aMessage);
				m_SEEDING_edgesRequested=true;
				m_SEEDING_edgesReceived=false;
			}else if(m_SEEDING_edgesReceived){
				VERTEX_TYPE vertexToVisit=m_dfsData->m_depthFirstSearchVerticesToVisit.top();
				int theDepth=m_dfsData->m_depthFirstSearchDepths.top();
				#ifdef DEBUG
				assert(theDepth>=0);
				assert(theDepth<=maxDepth);
				#endif
				int newDepth=theDepth+1;

				m_dfsData->m_depthFirstSearchVerticesToVisit.pop();
				m_dfsData->m_depthFirstSearchDepths.pop();


				#ifdef SHOW_MINI_GRAPH
				if(m_dfsData->m_coverages[vertexToVisit]>=m_minimumCoverage/2){
					string b=idToWord(vertexToVisit,m_wordSize);
					cout<<b<<" [label=\""<<b<<" "<<m_SEEDING_receivedVertexCoverage<<"\" ]"<<endl;
				}
				#endif

				for(int i=0;i<(int)m_SEEDING_receivedOutgoingEdges.size();i++){
					VERTEX_TYPE nextVertex=m_SEEDING_receivedOutgoingEdges[i];
					if(m_dfsData->m_depthFirstSearchVisitedVertices.count(nextVertex)>0)
						continue;
					if(newDepth>maxDepth)
						continue;
					m_dfsData->m_depthFirstSearchVerticesToVisit.push(nextVertex);
					m_dfsData->m_depthFirstSearchDepths.push(newDepth);
					if(m_dfsData->m_coverages[vertexToVisit]>=m_minimumCoverage/2){
						m_dfsData->m_depthFirstSearchVisitedVertices_vector.push_back(vertexToVisit);
						m_dfsData->m_depthFirstSearchVisitedVertices_vector.push_back(nextVertex);
						m_dfsData->m_depthFirstSearchVisitedVertices_depths.push_back(newDepth);

						#ifdef SHOW_MINI_GRAPH
						cout<<idToWord(vertexToVisit,m_wordSize)<<" -> "<<idToWord(nextVertex,m_wordSize)<<endl;
						#endif
					}
				}
				m_SEEDING_edgesRequested=false;
				m_SEEDING_vertexCoverageRequested=false;
			}
		}
	}else{
		m_dfsData->m_doChoice_tips_dfs_done=true;
		#ifdef SHOW_MINI_GRAPH
		cout<<"</MiniGraph>"<<endl;
		#endif
	}
}


void Machine::checkIfCurrentVertexIsAssembled(){
	if(!m_EXTENSION_directVertexDone){
		if(!m_EXTENSION_VertexAssembled_requested){
			if(m_EXTENSION_currentSeedIndex%10==0 and m_EXTENSION_currentPosition==0 and m_last_value!=m_EXTENSION_currentSeedIndex){
				m_last_value=m_EXTENSION_currentSeedIndex;
				#ifdef SHOW_PROGRESS
				cout<<"Rank "<<getRank()<<": extending seeds "<<m_EXTENSION_currentSeedIndex<<"/"<<m_SEEDING_seeds.size()<<endl;
				#endif
			}
			m_EXTENSION_VertexAssembled_requested=true;
			VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
			message[0]=(VERTEX_TYPE)m_SEEDING_currentVertex;
			Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(m_SEEDING_currentVertex),TAG_ASK_IS_ASSEMBLED,getRank());
			m_outbox.push_back(aMessage);
			m_EXTENSION_VertexAssembled_received=false;
		}else if(m_EXTENSION_VertexAssembled_received){
			m_EXTENSION_reverseVertexDone=false;
			m_EXTENSION_directVertexDone=true;
			m_EXTENSION_VertexMarkAssembled_requested=false;
			m_SEEDING_vertexCoverageRequested=false;
			m_EXTENSION_VertexAssembled_requested=false;
			if(m_EXTENSION_vertexIsAssembledResult){
				m_EXTENSION_checkedIfCurrentVertexIsAssembled=true;
				m_EXTENSION_markedCurrentVertexAsAssembled=false;
				m_EXTENSION_directVertexDone=false;
			}else{
			}
		}else{
		}
	}else if(!m_EXTENSION_reverseVertexDone){
		if(!m_EXTENSION_VertexAssembled_requested){
			m_EXTENSION_VertexAssembled_requested=true;
			VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
			message[0]=(VERTEX_TYPE)complementVertex(m_SEEDING_currentVertex,m_wordSize,m_colorSpaceMode);
			Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(message[0]),TAG_ASK_IS_ASSEMBLED,getRank());
			m_outbox.push_back(aMessage);
			m_EXTENSION_VertexAssembled_received=false;
		}else if(m_EXTENSION_VertexAssembled_received){
			m_EXTENSION_checkedIfCurrentVertexIsAssembled=true;
			m_EXTENSION_markedCurrentVertexAsAssembled=false;
			m_EXTENSION_directVertexDone=false;
		}else{
		}
	}
}

void Machine::markCurrentVertexAsAssembled(){
	if(!m_EXTENSION_directVertexDone){
		if(!m_SEEDING_vertexCoverageRequested){
			m_SEEDING_vertexCoverageRequested=true;
			m_SEEDING_vertexCoverageReceived=false;
			
			VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
			message[0]=m_SEEDING_currentVertex;
			Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(message[0]),TAG_REQUEST_VERTEX_COVERAGE,getRank());
			m_outbox.push_back(aMessage);
		}else if(m_SEEDING_vertexCoverageReceived){
			if(!m_EXTENSION_VertexMarkAssembled_requested){
				m_EXTENSION_extension.push_back(m_SEEDING_currentVertex);
				// save wave progress.
	
				int waveId=m_EXTENSION_currentSeedIndex*MAX_NUMBER_OF_MPI_PROCESSES+getRank();
				int progression=m_EXTENSION_extension.size()-1;
			

				m_EXTENSION_VertexMarkAssembled_requested=true;
				VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(3*sizeof(VERTEX_TYPE));
				message[0]=(VERTEX_TYPE)m_SEEDING_currentVertex;
				message[1]=waveId;
				message[2]=progression;
				Message aMessage(message,3,MPI_UNSIGNED_LONG_LONG,vertexRank(m_SEEDING_currentVertex),TAG_SAVE_WAVE_PROGRESSION,getRank());
				m_outbox.push_back(aMessage);
				m_EXTENSION_reverseVertexDone=true;
				m_EXTENSION_reads_requested=false;

			// get the reads starting at that position.
			}else if(!m_EXTENSION_reads_requested){
				m_EXTENSION_reads_requested=true;
				m_EXTENSION_reads_received=false;
				VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator.allocate(1*sizeof(VERTEX_TYPE));
				message[0]=(VERTEX_TYPE)m_SEEDING_currentVertex;
				Message aMessage(message,1,MPI_UNSIGNED_LONG_LONG,vertexRank(m_SEEDING_currentVertex),TAG_REQUEST_READS,getRank());
				m_outbox.push_back(aMessage);
			
			}else if(m_EXTENSION_reads_received){
				for(int i=0;i<(int)m_EXTENSION_receivedReads.size();i++){
					int uniqueId=m_EXTENSION_receivedReads[i].getUniqueId();
					// check that the complete sequence of m_SEEDING_currentVertex correspond to
					// the one of the start of the read on the good strand.
					// this is important when USE_DISTANT_SEGMENTS_GRAPH is set.

					if(m_EXTENSION_usedReads.count(uniqueId)==0 ){
						// use all reads available.
						if(true or m_SEEDING_receivedVertexCoverage<3*m_peakCoverage){
							m_EXTENSION_usedReads.insert(uniqueId);
							m_EXTENSION_reads_startingPositionOnContig[uniqueId]=m_EXTENSION_extension.size()-1;
							m_EXTENSION_readsInRange.insert(m_EXTENSION_receivedReads[i]);
							#ifdef DEBUG
							assert(m_EXTENSION_readsInRange.count(m_EXTENSION_receivedReads[i])>0);
							#endif
						}else{
							cout<<"Repeated vertex."<<endl;
						}
					}
				}
				m_EXTENSION_directVertexDone=true;
				m_EXTENSION_VertexMarkAssembled_requested=false;
				m_EXTENSION_enumerateChoices=false;
				m_SEEDING_edgesRequested=false;
				m_EXTENSION_markedCurrentVertexAsAssembled=true;
			}
		}
	}
}

int Machine::getSize(){
	return m_size;
}



bool Machine::isAlive(){
	return m_alive;
}

void Machine::printStatus(){
	cout<<"********"<<endl;
	cout<<"Rank: "<<getRank()<<endl;
	cout<<"Reads: "<<m_myReads.size()<<endl;
	cout<<"Inbox: "<<m_inbox.size()<<endl;
	cout<<"Outbox: "<<m_outbox.size()<<endl;
}

int Machine::vertexRank(VERTEX_TYPE a){
	return hash_VERTEX_TYPE(a)%(getSize());
}

Machine::~Machine(){
	// do nothing.
	delete m_dfsData;
	delete m_bubbleData;
	m_dfsData=NULL;
	m_bubbleData=NULL;
}

