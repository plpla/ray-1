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

 	Funding:
Sébastien Boisvert has a scholarship from the Canadian Institutes of Health Research (Master's award: 200910MDR-215249-172830 and Doctoral award: 200902CGM-204212-172830).

*/

#ifndef _mpi_tags
#define _mpi_tags

// tags for MPI
// these are the message types used by Ray
// Ray instances like to communicate a lots!
//

enum {
RAY_MPI_TAG_LOAD_SEQUENCES,
RAY_MPI_TAG_WELCOME,
RAY_MPI_TAG_SEQUENCES_READY,
RAY_MPI_TAG_VERTICES_DATA,
RAY_MPI_TAG_VERTICES_DISTRIBUTED,
RAY_MPI_TAG_OUT_EDGES_DATA,
RAY_MPI_TAG_SHOW_VERTICES,
RAY_MPI_TAG_START_VERTICES_DISTRIBUTION,
RAY_MPI_TAG_IN_EDGES_DATA,
RAY_MPI_TAG_PREPARE_COVERAGE_DISTRIBUTION_QUESTION,
RAY_MPI_TAG_PREPARE_COVERAGE_DISTRIBUTION_ANSWER,
RAY_MPI_TAG_PREPARE_COVERAGE_DISTRIBUTION,
RAY_MPI_TAG_COVERAGE_DATA,
RAY_MPI_TAG_COVERAGE_END,
RAY_MPI_TAG_SEND_COVERAGE_VALUES,
RAY_MPI_TAG_READY_TO_SEED,
RAY_MPI_TAG_START_SEEDING,
RAY_MPI_TAG_REQUEST_VERTEX_COVERAGE,
RAY_MPI_TAG_REQUEST_VERTEX_COVERAGE_REPLY,
RAY_MPI_TAG_REQUEST_VERTEX_KEY_AND_COVERAGE,
RAY_MPI_TAG_REQUEST_VERTEX_KEY_AND_COVERAGE_REPLY,
RAY_MPI_TAG_REQUEST_VERTEX_OUTGOING_EDGES,
RAY_MPI_TAG_REQUEST_VERTEX_EDGES,
RAY_MPI_TAG_REQUEST_VERTEX_OUTGOING_EDGES_REPLY,
RAY_MPI_TAG_SEEDING_IS_OVER,
RAY_MPI_TAG_GOOD_JOB_SEE_YOU_SOON,
RAY_MPI_TAG_I_GO_NOW,
RAY_MPI_TAG_MASTER_IS_DONE_ATTACHING_READS,
RAY_MPI_TAG_MASTER_IS_DONE_ATTACHING_READS_REPLY,
RAY_MPI_TAG_REQUEST_VERTEX_INGOING_EDGES,
RAY_MPI_TAG_REQUEST_VERTEX_INGOING_EDGES_REPLY,
RAY_MPI_TAG_EXTENSION_IS_DONE,
RAY_MPI_TAG_ASK_EXTENSION,
RAY_MPI_TAG_ASK_IS_ASSEMBLED,
RAY_MPI_TAG_ASK_IS_ASSEMBLED_REPLY,
RAY_MPI_TAG_MARK_AS_ASSEMBLED,
RAY_MPI_TAG_ASK_EXTENSION_DATA,
RAY_MPI_TAG_EXTENSION_DATA,
RAY_MPI_TAG_EXTENSION_END,
RAY_MPI_TAG_EXTENSION_DATA_END,
RAY_MPI_TAG_ATTACH_SEQUENCE,
RAY_MPI_TAG_REQUEST_READS,
RAY_MPI_TAG_REQUEST_READS_REPLY,
RAY_MPI_TAG_ASK_READ_VERTEX_AT_POSITION,
RAY_MPI_TAG_ASK_READ_VERTEX_AT_POSITION_REPLY,
RAY_MPI_TAG_ASK_READ_LENGTH,
RAY_MPI_TAG_ASK_READ_LENGTH_REPLY,
RAY_MPI_TAG_SAVE_WAVE_PROGRESSION,
RAY_MPI_TAG_COPY_DIRECTIONS,
RAY_MPI_TAG_ASSEMBLE_WAVES,
RAY_MPI_TAG_SAVE_WAVE_PROGRESSION_REVERSE,
RAY_MPI_TAG_ASSEMBLE_WAVES_DONE,
RAY_MPI_TAG_START_FUSION,
RAY_MPI_TAG_FUSION_DONE,
RAY_MPI_TAG_ASK_VERTEX_PATHS_SIZE,
RAY_MPI_TAG_ASK_VERTEX_PATHS_SIZE_REPLY,
RAY_MPI_TAG_GET_PATH_LENGTH,
RAY_MPI_TAG_GET_PATH_LENGTH_REPLY,
RAY_MPI_TAG_CALIBRATION_MESSAGE,
RAY_MPI_TAG_BEGIN_CALIBRATION,
RAY_MPI_TAG_END_CALIBRATION,
RAY_MPI_TAG_COMMUNICATION_STABILITY_MESSAGE,
RAY_MPI_TAG_ASK_VERTEX_PATH,
RAY_MPI_TAG_ASK_VERTEX_PATH_REPLY,
RAY_MPI_TAG_INDEX_PAIRED_SEQUENCE,
RAY_MPI_TAG_HAS_PAIRED_READ,
RAY_MPI_TAG_HAS_PAIRED_READ_REPLY,
RAY_MPI_TAG_GET_PAIRED_READ,
RAY_MPI_TAG_GET_PAIRED_READ_REPLY,
RAY_MPI_TAG_CLEAR_DIRECTIONS,
RAY_MPI_TAG_CLEAR_DIRECTIONS_REPLY,
RAY_MPI_TAG_FINISH_FUSIONS,
RAY_MPI_TAG_FINISH_FUSIONS_FINISHED,
RAY_MPI_TAG_DISTRIBUTE_FUSIONS,
RAY_MPI_TAG_DISTRIBUTE_FUSIONS_FINISHED,
RAY_MPI_TAG_EXTENSION_START,
RAY_MPI_TAG_ELIMINATE_PATH,
RAY_MPI_TAG_GET_PATH_VERTEX,
RAY_MPI_TAG_GET_PATH_VERTEX_REPLY,
RAY_MPI_TAG_AUTOMATIC_DISTANCE_DETECTION,
RAY_MPI_TAG_AUTOMATIC_DISTANCE_DETECTION_IS_DONE,
RAY_MPI_TAG_LIBRARY_DISTANCE,
RAY_MPI_TAG_ASK_LIBRARY_DISTANCES,
RAY_MPI_TAG_ASK_LIBRARY_DISTANCES_FINISHED,
RAY_MPI_TAG_UPDATE_LIBRARY_INFORMATION,
RAY_MPI_TAG_RECEIVED_COVERAGE_INFORMATION,
RAY_MPI_TAG_REQUEST_READ_SEQUENCE,
RAY_MPI_TAG_REQUEST_READ_SEQUENCE_REPLY,
RAY_MPI_TAG_SEND_SEQUENCE_REPLY,
RAY_MPI_TAG_SAVE_WAVE_PROGRESSION_REPLY,
RAY_MPI_TAG_SEND_SEQUENCE_REGULATOR,
RAY_MPI_TAG_START_INDEXING_SEQUENCES,
RAY_MPI_TAG_VERTICES_DATA_REPLY,
RAY_MPI_TAG_IN_EDGES_DATA_REPLY,
RAY_MPI_TAG_OUT_EDGES_DATA_REPLY,
RAY_MPI_TAG_INDEX_PAIRED_SEQUENCE_REPLY,
RAY_MPI_TAG_EXTENSION_DATA_REPLY,
RAY_MPI_TAG_LIBRARY_DISTANCE_REPLY,
RAY_MPI_TAG_RECEIVED_MESSAGES,
RAY_MPI_TAG_RECEIVED_MESSAGES_REPLY,
RAY_MPI_TAG_DISTRIBUTE_FUSIONS_FINISHED_REPLY,
RAY_MPI_TAG_DISTRIBUTE_FUSIONS_FINISHED_REPLY_REPLY,
RAY_MPI_TAG_SAVE_WAVE_PROGRESSION_WITH_REPLY,
RAY_MPI_TAG_ASK_IS_ASSEMBLED_REPLY_END,
RAY_MPI_TAG_MUST_RUN_REDUCER,
RAY_MPI_TAG_ASK_BEGIN_REDUCTION,
RAY_MPI_TAG_ASK_BEGIN_REDUCTION_REPLY,
RAY_MPI_TAG_START_REDUCTION,
RAY_MPI_TAG_REDUCE_MEMORY_CONSUMPTION_DONE,
RAY_MPI_TAG_RESUME_VERTEX_DISTRIBUTION,
RAY_MPI_TAG_ASK_BEGIN_REDUCTION_REPLY_FLUSHED,
RAY_MPI_TAG_ASK_BEGIN_REDUCTION_REPLY_FLUSHED_REPLY,
RAY_MPI_TAG_DELETE_VERTICES_DONE,
RAY_MPI_TAG_DELETE_VERTICES,
RAY_MPI_TAG_UPDATE_THRESHOLD,
RAY_MPI_TAG_UPDATE_THRESHOLD_REPLY,
RAY_MPI_TAG_DELETE_INGOING_EDGE,
RAY_MPI_TAG_DELETE_OUTGOING_EDGE,
RAY_MPI_TAG_DELETE_VERTEX,
RAY_MPI_TAG_DELETE_VERTEX_REPLY,
RAY_MPI_TAG_DELETE_OUTGOING_EDGE_REPLY,
RAY_MPI_TAG_DELETE_INGOING_EDGE_REPLY,
RAY_MPI_TAG_CHECK_VERTEX,
RAY_MPI_TAG_CHECK_VERTEX_REPLY,
RAY_MPI_TAG_ATTACH_SEQUENCE_REPLY,
RAY_MPI_TAG_SET_WORD_SIZE,
RAY_MPI_TAG_MUST_RUN_REDUCER_FROM_MASTER
};

#endif
