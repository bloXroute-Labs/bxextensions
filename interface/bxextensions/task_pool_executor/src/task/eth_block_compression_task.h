#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <memory>

#include <tpe/task/eth_block_compression_task.h>

#ifndef SRC_TASK_ETH_COMPRESSION_TASK_H_
#define SRC_TASK_ETH_COMPRESSION_TASK_H_

typedef task::EthBlockCompressionTask EthBlockCompressionTask_t;
typedef std::shared_ptr<EthBlockCompressionTask_t> PEthBlockCompressionTask_t;
typedef utils::crypto::Sha256 Sha256_t;
typedef std::shared_ptr<Sha256_t> PSha256_t;

void bind_eth_block_compression_task(py::module& m);



#endif /* SRC_TASK_ETH_COMPRESSION_TASK_H_ */
