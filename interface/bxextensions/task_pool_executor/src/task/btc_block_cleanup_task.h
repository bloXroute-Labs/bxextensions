#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>
#include <memory>

#include <tpe/task/btc_block_cleanup_task.h>

#ifndef SRC_TASK_BTC_BLOCK_CLEANUP_TASK_H_
#define SRC_TASK_BTC_BLOCK_CLEANUP_TASK_H_

typedef task::BtcBlockCleanupTask BtcBlockCleanupTask_t;
typedef std::shared_ptr<BtcBlockCleanupTask_t> PBtcBlockCleanupTask_t;

void bind_btc_block_cleanup_task(py::module& m);

#endif /* SRC_TASK_BTC_BLOCK_CLEANUP_TASK_H_ */
