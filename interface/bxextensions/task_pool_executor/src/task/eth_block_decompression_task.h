#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <memory>

#include <tpe/task/eth_block_decompression_task.h>

#ifndef SRC_TASK_ETH_BLOCK_DECOMPRESSION_TASK_H_
#define SRC_TASK_ETH_BLOCK_DECOMPRESSION_TASK_H_

typedef task::EthBlockDecompressionTask EthBlockDecompressionTask_t;
typedef std::shared_ptr<EthBlockDecompressionTask_t> PEthBlockDecompressionTask_t;

void bind_eth_block_decompression_task(py::module& m);

#endif /* SRC_TASK_ETH_BLOCK_DECOMPRESSION_TASK_H_ */
