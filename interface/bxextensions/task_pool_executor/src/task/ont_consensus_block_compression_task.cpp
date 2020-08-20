#include "src/task/main_task_base.h"
#include "src/task/ont_consensus_block_compression_task.h"

#include <utils/crypto/hash_helper.h>

void bind_ont_consensus_block_compression_task(py::module& m) {
    py::class_<
            OntConsensusBlockCompressionTask_t,
            MainTaskBase_t,
            POntConsensusBlockCompressionTask_t>(m, "OntConsensusBlockCompressionTask")
            .def(py::init<size_t, size_t>(),
                 "initializing",
                 py::arg("capacity") = ONT_DEFAULT_BLOCK_SIZE,
                 py::arg("minimal_tx_count") = ONT_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT)
            .def(
                    "bx_block",
                    &OntConsensusBlockCompressionTask_t::bx_block,
                    py::return_value_policy::reference
            )
            .def("init", &OntConsensusBlockCompressionTask_t::init)
            .def("prev_block_hash", &OntConsensusBlockCompressionTask_t::prev_block_hash)
            .def("block_hash", &OntConsensusBlockCompressionTask_t::block_hash)
            .def("compressed_block_hash", &OntConsensusBlockCompressionTask_t::compressed_block_hash)
            .def("txn_count", &OntConsensusBlockCompressionTask_t::txn_count)
            .def("short_ids", &OntConsensusBlockCompressionTask_t::short_ids)
            .def("ignored_short_ids", &OntConsensusBlockCompressionTask_t::ignored_short_ids);
}
