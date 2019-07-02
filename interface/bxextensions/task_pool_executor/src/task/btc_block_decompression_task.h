#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <memory>

#include <tpe/task/btc_block_decompression_task.h>

#ifndef SRC_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_
#define SRC_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_

typedef task::BtcBlockDecompressionTask BtcBlockDecompressionTask_t;
typedef std::shared_ptr<BtcBlockDecompressionTask_t> PBtcBlockDecompressionTask_t;

void bind_btc_block_decompression_task(py::module& m);

#endif /* SRC_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_ */
