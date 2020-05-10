#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>
#include <memory>

#include <tpe/task/ont_block_cleanup_task.h>

#ifndef SRC_TASK_ONT_BLOCK_CLEANUP_TASK_H_
#define SRC_TASK_ONT_BLOCK_CLEANUP_TASK_H_

typedef task::OntBlockCleanupTask OntBlockCleanupTask_t;
typedef std::shared_ptr<OntBlockCleanupTask_t> POntBlockCleanupTask_t;

void bind_ont_block_cleanup_task(py::module& m);

#endif /* SRC_TASK_ONT_BLOCK_CLEANUP_TASK_H_ */
