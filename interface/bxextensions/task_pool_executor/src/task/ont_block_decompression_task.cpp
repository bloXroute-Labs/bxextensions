#include "src/task/main_task_base.h"
#include "src/task/ont_block_decompression_task.h"

#include <utils/crypto/hash_helper.h>


void bind_ont_block_decompression_task(py::module& m) {
    py::class_<
            OntBlockDecompressionTask_t,
            MainTaskBase_t,
            POntBlockDecompressionTask_t>(m, "OntBlockDecompressionTask")
            .def(py::init<size_t, size_t>(),
                 "initializing",
                 py::arg("capacity") = ONT_DEFAULT_BLOCK_SIZE,
                 py::arg("minimal_tx_count") = ONT_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT
            )
            .def(
                    "block_message",
                    &OntBlockDecompressionTask_t::btc_block,
                    py::return_value_policy::reference
            )
            .def("block_hash", &OntBlockDecompressionTask_t::block_hash)
            .def("tx_count", &OntBlockDecompressionTask_t::tx_count)
            .def("success", &OntBlockDecompressionTask_t::success)
            .def(
                    "short_ids",
                    &OntBlockDecompressionTask_t::short_ids
            )
            .def(
                    "unknown_tx_hashes",
                    &OntBlockDecompressionTask_t::unknown_tx_hashes
            )
            .def(
                    "unknown_tx_sids",
                    &OntBlockDecompressionTask_t::unknown_tx_sids
            )
            .def("init", &OntBlockDecompressionTask_t::init);
}
