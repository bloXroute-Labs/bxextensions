#include "src/task/main_task_base.h"
#include "src/task/eth_block_compression_task.h"

#include <utils/crypto/hash_helper.h>

void bind_eth_block_compression_task(py::module& m) {
	py::class_<
	EthBlockCompressionTask_t,
	MainTaskBase_t,
	PEthBlockCompressionTask_t>(m, "EthBlockCompressionTask")
	  .def(py::init<size_t, size_t>(),
			  "initializing",
			  py::arg("capacity") = ETH_DEFAULT_BLOCK_SIZE,
			  py::arg("minimal_tx_count") = ETH_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT)
	  .def(
			  "bx_block",
			  &EthBlockCompressionTask_t::bx_block,
			  py::return_value_policy::reference
	  )
	  .def("init", &EthBlockCompressionTask_t::init)
	  .def("prev_block_hash", &EthBlockCompressionTask_t::prev_block_hash)
	  .def("block_hash", &EthBlockCompressionTask_t::block_hash)
	  .def("compressed_block_hash", &EthBlockCompressionTask_t::compressed_block_hash)
	  .def("txn_count", &EthBlockCompressionTask_t::txn_count)
	  .def("short_ids", &EthBlockCompressionTask_t::short_ids);
}
