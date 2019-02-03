#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <tpe/task/main_task_base.h>

#ifndef SRC_TASK_MAIN_TASK_BASE_H_
#define SRC_TASK_MAIN_TASK_BASE_H_

typedef task::MainTaskBase MainTaskBase_t;
typedef std::shared_ptr<MainTaskBase_t> PMainTaskBase_t;

void bind_main_task_base(py::module& m);

#endif /* SRC_TASK_MAIN_TASK_BASE_H_ */
