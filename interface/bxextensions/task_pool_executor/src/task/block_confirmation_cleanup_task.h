#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>
#include <memory>

#include <tpe/task/block_confirmation_cleanup_task.h>

#ifndef SRC_TASK_BLOCK_CONFIRMATION_CLEANUP_TASK_H
#define SRC_TASK_BLOCK_CONFIRMATION_CLEANUP_TASK_H

typedef task::BlockConfirmationCleanupTask BlockConfirmationCleanupTask_t;
typedef std::shared_ptr<BlockConfirmationCleanupTask_t> PBlockConfirmationCleanupTask_t;

void bind_block_confirmation_cleanup_task(py::module& m);

#endif /* SRC_TASK_BLOCK_CONFIRMATION_CLEANUP_TASK_H */
