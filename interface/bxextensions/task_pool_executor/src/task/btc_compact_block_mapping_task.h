#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <memory>

#include <tpe/task/btc_compact_block_mapping_task.h>

#ifndef SRC_TASK_BTC_COMPACT_BLOCK_MAPPING_TASK_H_
#define SRC_TASK_BTC_COMPACT_BLOCK_MAPPING_TASK_H_

typedef task::BtcCompactBlockMappingTask BtcCompactBlockMappingTask_t;
typedef std::shared_ptr<BtcCompactBlockMappingTask_t> PBtcCompactBlockMappingTask_t;

void bind_btc_compact_block_mapping_task(py::module& m);


#endif /* SRC_TASK_BTC_COMPACT_BLOCK_MAPPING_TASK_H_ */
