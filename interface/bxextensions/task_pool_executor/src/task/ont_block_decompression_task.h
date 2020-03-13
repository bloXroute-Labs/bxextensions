#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <memory>

#include <tpe/task/ont_block_decompression_task.h>

#ifndef SRC_TASK_ONT_BLOCK_DECOMPRESSION_TASK_H_
#define SRC_TASK_ONT_BLOCK_DECOMPRESSION_TASK_H_

typedef task::OntBlockDecompressionTask OntBlockDecompressionTask_t;
typedef std::shared_ptr<OntBlockDecompressionTask_t> POntBlockDecompressionTask_t;

void bind_ont_block_decompression_task(py::module& m);

#endif //SRC_TASK_ONT_BLOCK_DECOMPRESSION_TASK_H_
