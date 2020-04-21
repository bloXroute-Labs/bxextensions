#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <memory>

#include <tpe/task/consensus_ont_block_compression_task.h>

#ifndef SRC_TASK_CONSENSUS_ONT_BLOCK_COMPRESSION_TASK_H_
#define SRC_TASK_CONSENSUS_ONT_BLOCK_COMPRESSION_TASK_H_

typedef task::ConsensusOntBlockCompressionTask ConsensusOntBlockCompressionTask_t;
typedef std::shared_ptr<ConsensusOntBlockCompressionTask_t> PConsensusOntBlockCompressionTask_t;
typedef utils::crypto::Sha256 Sha256_t;
typedef std::shared_ptr<Sha256_t> PSha256_t;

void bind_consensus_ont_block_compression_task(py::module& m);

#endif //SRC_TASK_CONSENSUS_ONT_BLOCK_COMPRESSION_TASK_H_
