#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <memory>

#include <tpe/task/btc_compact_block_mapping_task.h>

#ifndef SRC_TASK_BTC_COMPACT_BLOCK_COMPRESSION_TASK_H_
#define SRC_TASK_BTC_COMPACT_BLOCK_COMPRESSION_TASK_H_


typedef task::BtcCompactBlockCompressionTask BtcCompactBlockCompressionTask_t;
typedef std::shared_ptr<BtcCompactBlockCompressionTask_t> PBtcCompactBlockCompressionTask_t;

void bind_btc_compact_block_compression_task(py::module& m);


#endif /* SRC_TASK_BTC_COMPACT_BLOCK_COMPRESSION_TASK_H_ */
