#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <tpe/task/task_base.h>

#ifndef SRC_TASKS_TASK_BASE_H_
#define SRC_TASKS_TASK_BASE_H_

typedef task::TaskBase TaskBase_t;
typedef std::shared_ptr<TaskBase_t> PTaskBase_t;

void bind_task_base(py::module& m);

#endif /* SRC_TASKS_TASK_BASE_H_ */
