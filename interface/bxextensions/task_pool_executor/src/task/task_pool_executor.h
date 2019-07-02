#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#ifndef SRC_TASK_TASK_POOL_EXECUTOR_H
#define SRC_TASK_TASK_POOL_EXECUTOR_H

void bind_tpe(py::module& m);

#endif //SRC_TASK_TASK_POOL_EXECUTOR_H
