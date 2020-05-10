#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <memory>

#include <tpe/task/ont_consensus_block_decompression_task.h>

#ifndef SRC_TASK_ONT_CONSENSUS_BLOCK_DECOMPRESSION_TASK_H_
#define SRC_TASK_ONT_CONSENSUS_BLOCK_DECOMPRESSION_TASK_H_

typedef task::OntConsensusBlockDecompressionTask OntConsensusBlockDecompressionTask_t;
typedef std::shared_ptr<OntConsensusBlockDecompressionTask_t> POntConsensusBlockDecompressionTask_t;

void bind_ont_consensus_block_decompression_task(py::module& m);

#endif //SRC_TASK_ONT_CONSENSUS_BLOCK_DECOMPRESSION_TASK_H_
