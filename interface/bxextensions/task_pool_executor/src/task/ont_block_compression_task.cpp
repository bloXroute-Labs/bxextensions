#include "src/task/main_task_base.h"
#include "src/task/ont_block_compression_task.h"

#include <utils/crypto/hash_helper.h>

void bind_ont_block_compression_task(py::module& m) {
    py::class_<
            OntBlockCompressionTask_t,
            MainTaskBase_t,
            POntBlockCompressionTask_t>(m, "OntBlockCompressionTask")
            .def(py::init<size_t, size_t>(),
                 "initializing",
                 py::arg("capacity") = ONT_DEFAULT_BLOCK_SIZE,
                 py::arg("minimal_tx_count") = ONT_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT)
            .def(
                    "bx_block",
                    &OntBlockCompressionTask_t::bx_block,
                    py::return_value_policy::reference
            )
            .def("init", &OntBlockCompressionTask_t::init)
            .def("prev_block_hash", &OntBlockCompressionTask_t::prev_block_hash)
            .def("block_hash", &OntBlockCompressionTask_t::block_hash)
            .def("compressed_block_hash", &OntBlockCompressionTask_t::compressed_block_hash)
            .def("txn_count", &OntBlockCompressionTask_t::txn_count)
            .def(
                    "short_ids",
                    &OntBlockCompressionTask_t::short_ids
            );
}
