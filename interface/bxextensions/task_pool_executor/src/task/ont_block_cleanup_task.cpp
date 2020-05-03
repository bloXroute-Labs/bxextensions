#include "src/task/main_task_base.h"
#include "src/task/ont_block_cleanup_task.h"

#include <utils/crypto/hash_helper.h>


void bind_ont_block_cleanup_task(py::module& m) {
    py::class_<
            OntBlockCleanupTask_t ,
            MainTaskBase_t,
            POntBlockCleanupTask_t>(m, "OntBlockCleanupTask")
            .def(py::init<size_t>(),
                 "initializing",
                 py::arg("minimal_tx_count") = ONT_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT
            )
            .def(
                    "short_ids",
                    &OntBlockCleanupTask_t::short_ids
            )
            .def(
                    "txn_count",
                    &OntBlockCleanupTask_t::txn_count
            )
            .def(
                    "unknown_tx_hashes",
                    &OntBlockCleanupTask_t::unknown_tx_hashes
            )
            .def(
                    "total_content_removed",
                    &OntBlockCleanupTask_t::total_content_removed
            )
            .def("init", &OntBlockCleanupTask_t::init);
}
