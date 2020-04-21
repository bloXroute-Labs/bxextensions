#include "src/task/main_task_base.h"
#include "src/task/consensus_ont_block_compression_task.h"

#include <utils/crypto/hash_helper.h>

void bind_consensus_ont_block_compression_task(py::module& m) {
    py::class_<
            ConsensusOntBlockCompressionTask_t,
            MainTaskBase_t,
            PConsensusOntBlockCompressionTask_t>(m, "ConsensusOntBlockCompressionTask")
            .def(py::init<size_t, size_t>(),
                 "initializing",
                 py::arg("capacity") = ONT_DEFAULT_BLOCK_SIZE,
                 py::arg("minimal_tx_count") = ONT_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT)
            .def(
                    "bx_block",
                    &ConsensusOntBlockCompressionTask_t::bx_block,
                    py::return_value_policy::reference
            )
            .def("init", &ConsensusOntBlockCompressionTask_t::init)
            .def("prev_block_hash", &ConsensusOntBlockCompressionTask_t::prev_block_hash)
            .def("block_hash", &ConsensusOntBlockCompressionTask_t::block_hash)
            .def("compressed_block_hash", &ConsensusOntBlockCompressionTask_t::compressed_block_hash)
            .def("txn_count", &ConsensusOntBlockCompressionTask_t::txn_count)
            .def(
                    "short_ids",
                    &ConsensusOntBlockCompressionTask_t::short_ids
            );
}
