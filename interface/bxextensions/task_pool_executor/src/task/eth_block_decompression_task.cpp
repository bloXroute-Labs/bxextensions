#include "src/task/main_task_base.h"
#include "src/task/eth_block_decompression_task.h"

#include <utils/crypto/hash_helper.h>


void bind_eth_block_decompression_task(py::module& m) {
	py::class_<
	EthBlockDecompressionTask_t,
	MainTaskBase_t,
	PEthBlockDecompressionTask_t>(m, "EthBlockDecompressionTask")
	  .def(py::init<size_t, size_t>(),
			  "initializing",
			  py::arg("capacity") = ETH_DEFAULT_BLOCK_SIZE,
			  py::arg("minimal_tx_count") = ETH_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT
	  )
	  .def(
			  "block_message",
			  &EthBlockDecompressionTask_t::eth_block,
			  py::return_value_policy::reference
	  )
	  .def("block_hash", &EthBlockDecompressionTask_t::block_hash)
	  .def("tx_count", &EthBlockDecompressionTask_t::tx_count)
	  .def("success", &EthBlockDecompressionTask_t::success)
	  .def("short_ids", &EthBlockDecompressionTask_t::short_ids)
	  .def("unknown_tx_hashes", &EthBlockDecompressionTask_t::unknown_tx_hashes)
	  .def("unknown_tx_sids", &EthBlockDecompressionTask_t::unknown_tx_sids)
	  .def("init", &EthBlockDecompressionTask_t::init);
}
