#include "src/task/main_task_base.h"
#include "src/task/ont_consensus_block_decompression_task.h"

#include <utils/crypto/hash_helper.h>


void bind_ont_consensus_block_decompression_task(py::module& m) {
    py::class_<
            OntConsensusBlockDecompressionTask_t,
            MainTaskBase_t,
            POntConsensusBlockDecompressionTask_t>(m, "OntConsensusBlockDecompressionTask")
            .def(py::init<size_t, size_t>(),
                 "initializing",
                 py::arg("capacity") = ONT_DEFAULT_BLOCK_SIZE,
                 py::arg("minimal_tx_count") = ONT_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT
            )
            .def(
                    "block_message",
                    &OntConsensusBlockDecompressionTask_t::block_message,
                    py::return_value_policy::reference
            )
            .def("block_hash", &OntConsensusBlockDecompressionTask_t::block_hash)
            .def("tx_count", &OntConsensusBlockDecompressionTask_t::txn_count)
            .def("success", &OntConsensusBlockDecompressionTask_t::success)
            .def(
                    "short_ids",
                    &OntConsensusBlockDecompressionTask_t::short_ids
            )
            .def(
                    "unknown_tx_hashes",
                    &OntConsensusBlockDecompressionTask_t::unknown_tx_hashes
            )
            .def(
                    "unknown_tx_sids",
                    &OntConsensusBlockDecompressionTask_t::unknown_tx_sids
            )
            .def("init", &OntConsensusBlockDecompressionTask_t::init);
}
