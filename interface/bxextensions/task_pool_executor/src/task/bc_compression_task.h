#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <memory>

#include <tpe/task/bc_compression_task.h>

#ifndef SRC_TASK_BC_COMPRESSION_TASK_H_
#define SRC_TASK_BC_COMPRESSION_TASK_H_

typedef task::BCCompressionTask BCCompressionTask_t;
typedef std::shared_ptr<BCCompressionTask_t> PBCCompressionTask_t;

void bind_bc_compression_task(py::module& m);



#endif /* SRC_TASK_BC_COMPRESSION_TASK_H_ */
