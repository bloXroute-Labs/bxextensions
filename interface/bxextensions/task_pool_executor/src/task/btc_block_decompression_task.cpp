#include "src/task/main_task_base.h"
#include "src/task/btc_block_decompression_task.h"

#include <utils/crypto/hash_helper.h>


void bind_btc_block_decompression_task(py::module& m) {
	py::class_<
	BTCBlockDecompressionTask_t,
	MainTaskBase_t,
	PBTCBlockDecompressionTask_t>(m, "BTCBlockDecompressionTask")
	  .def(py::init<
			  const task::ShortIDToSha256Map_t&,
			  const task::Sha256ToTxMap_t&,
			  size_t
			  >(),
			  "initializing",
			  py::arg("short_id_map"),
			  py::arg("tx_map"),
			  py::arg("capacity") = BTC_DEFAULT_BLOCK_SIZE)
	  .def(
			  "block_message",
			  &BTCBlockDecompressionTask_t::block_message,
			  py::return_value_policy::reference
	  )
	  .def("block_hash", &BTCBlockDecompressionTask_t::block_hash)
	  .def("unknown_tx_hashes", &BTCBlockDecompressionTask_t::unknown_tx_hashes)
	  .def("unknown_tx_sids", &BTCBlockDecompressionTask_t::unknown_tx_sids)
	  .def("init", &BTCBlockDecompressionTask_t::init);
}
