#include "src/task/block_confirmation_cleanup_task.h"
#include "src/task/main_task_base.h"

void bind_block_confirmation_cleanup_task(py::module& m) {
    py::class_<
            BlockConfirmationCleanupTask_t,
            MainTaskBase_t,
            PBlockConfirmationCleanupTask_t>(m, "BlockConfirmationCleanupTask")
            .def(py::init<>(),
                 "initializing"
            )
            .def(
                    "short_ids",
                    &BlockConfirmationCleanupTask_t::short_ids
            )
            .def(
                    "tx_count",
                    &BlockConfirmationCleanupTask_t::tx_count
            )
            .def(
                    "block_hash",
                    &BlockConfirmationCleanupTask_t::block_hash
            )
            .def(
                    "total_content_removed",
                    &BlockConfirmationCleanupTask_t::total_content_removed
            )
            .def("init", &BlockConfirmationCleanupTask_t::init);
}

