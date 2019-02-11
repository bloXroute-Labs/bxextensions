#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <memory>

#include <tpe/task/btc_block_decompression_task.h>

#ifndef SRC_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_
#define SRC_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_

typedef task::BTCBlockDecompressionTask BTCBlockDecompressionTask_t;
typedef std::shared_ptr<BTCBlockDecompressionTask_t> PBTCBlockDecompressionTask_t;

void bind_btc_block_decompression_task(py::module& m);

#endif /* SRC_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_ */
