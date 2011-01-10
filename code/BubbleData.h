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

#ifndef _BubbleData
#define _BubbleData

#include<vector>
#include<map>
#include<common_functions.h>
#include<stdio.h>
#include<stdlib.h>
#include<set>
using namespace std;

/*
 * put some members in this class or else g++ don't like it. it makes otherwise the program segfault!
 */
class BubbleData{
public:
	// arcs with good coverage
	std::vector<std::vector<uint64_t> > m_BUBBLE_visitedVertices;
	// all vertices.
	std::vector<std::set<uint64_t> > m_visitedVertices;
	bool m_doChoice_bubbles_Detected;
	bool m_doChoice_bubbles_Initiated;
	std::vector<std::vector<int> > m_BUBBLE_visitedVerticesDepths;
	map<uint64_t,int> m_coverages;
	FILE*m_amos;
};

#endif
