#include "src/task/main_task_base.h"
#include "src/task/btc_block_decompression_task.h"

#include <utils/crypto/hash_helper.h>


void bind_btc_block_decompression_task(py::module& m) {
	py::class_<
	BtcBlockDecompressionTask_t,
	MainTaskBase_t,
	PBtcBlockDecompressionTask_t>(m, "BtcBlockDecompressionTask")
	  .def(py::init<size_t, size_t>(),
			  "initializing",
			  py::arg("capacity") = BTC_DEFAULT_BLOCK_SIZE,
			  py::arg("minimal_tx_count") = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT
	  )
	  .def(
			  "block_message",
			  &BtcBlockDecompressionTask_t::btc_block,
			  py::return_value_policy::reference
	  )
	  .def("block_hash", &BtcBlockDecompressionTask_t::block_hash)
	  .def("tx_count", &BtcBlockDecompressionTask_t::tx_count)
	  .def("success", &BtcBlockDecompressionTask_t::success)
	  .def(
			  "short_ids",
			  &BtcBlockDecompressionTask_t::short_ids
	  )
	  .def(
			  "unknown_tx_hashes",
			  &BtcBlockDecompressionTask_t::unknown_tx_hashes
	  )
	  .def(
			  "unknown_tx_sids",
			  &BtcBlockDecompressionTask_t::unknown_tx_sids
	  )
	  .def("init", &BtcBlockDecompressionTask_t::init);
}
