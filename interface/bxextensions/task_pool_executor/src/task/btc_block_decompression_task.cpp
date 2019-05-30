#include "src/task/main_task_base.h"
#include "src/task/btc_block_decompression_task.h"

#include <utils/crypto/hash_helper.h>


void bind_btc_block_decompression_task(py::module& m) {
	py::class_<
	BTCBlockDecompressionTask_t,
	MainTaskBase_t,
	PBTCBlockDecompressionTask_t>(m, "BTCBlockDecompressionTask")
	  .def(py::init<size_t, size_t>(),
			  "initializing",
			  py::arg("capacity") = BTC_DEFAULT_BLOCK_SIZE,
			  py::arg("minimal_tx_count") = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT
	  )
	  .def(
			  "block_message",
			  &BTCBlockDecompressionTask_t::btc_block,
			  py::return_value_policy::reference
	  )
	  .def("block_hash", &BTCBlockDecompressionTask_t::block_hash)
	  .def("tx_count", &BTCBlockDecompressionTask_t::tx_count)
	  .def("success", &BTCBlockDecompressionTask_t::success)
	  .def(
			  "short_ids",
			  &BTCBlockDecompressionTask_t::short_ids
	  )
	  .def(
			  "unknown_tx_hashes",
			  &BTCBlockDecompressionTask_t::unknown_tx_hashes
	  )
	  .def(
			  "unknown_tx_sids",
			  &BTCBlockDecompressionTask_t::unknown_tx_sids
	  )
	  .def("init", &BTCBlockDecompressionTask_t::init);
}
