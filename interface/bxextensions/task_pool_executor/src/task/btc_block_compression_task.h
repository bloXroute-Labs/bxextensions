#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <memory>

#include <tpe/task/btc_block_compression_task.h>

#ifndef SRC_TASK_BC_COMPRESSION_TASK_H_
#define SRC_TASK_BC_COMPRESSION_TASK_H_

typedef task::BtcBlockCompressionTask BtcBlockCompressionTask_t;
typedef std::shared_ptr<BtcBlockCompressionTask_t> PBtcBlockCompressionTask_t;
typedef utils::crypto::Sha256 Sha256_t;
typedef std::shared_ptr<Sha256_t> PSha256_t;

void bind_btc_block_compression_task(py::module& m);



#endif /* SRC_TASK_BC_COMPRESSION_TASK_H_ */
