#include "src/task/main_task_base.h"
#include "src/task/btc_block_compression_task.h"

#include <utils/crypto/hash_helper.h>

void bind_btc_block_compression_task(py::module& m) {
	py::class_<utils::crypto::Sha256>(m, "Sha256")
			.def(py::init<const utils::common::BufferView&>())
			.def("__repr__", &utils::crypto::Sha256::repr)
			.def_property_readonly(
					"binary",
					 &utils::crypto::Sha256::sha256
			)
			.def("__len__", &utils::crypto::Sha256::size)
			.def("hex_string", &utils::crypto::Sha256::repr);

	m.def("double_sha256", [](const utils::common::BufferView& buf) {
		return utils::crypto::double_sha256(buf, 0, buf.size());
	});

	py::class_<
	BTCBlockCompressionTask_t,
	MainTaskBase_t,
	PBTCBlockCompressionTask_t>(m, "BTCBlockCompressionTask")
	  .def(py::init<size_t, size_t>(),
			  "initializing",
			  py::arg("capacity") = BTC_DEFAULT_BLOCK_SIZE,
			  py::arg("minimal_tx_count") = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT)
	  .def(
			  "bx_block",
			  &BTCBlockCompressionTask_t::bx_block,
			  py::return_value_policy::reference
	  )
	  .def("init", &BTCBlockCompressionTask_t::init)
	  .def("prev_block_hash", &BTCBlockCompressionTask_t::prev_block_hash)
	  .def("block_hash", &BTCBlockCompressionTask_t::block_hash)
	  .def("compressed_block_hash", &BTCBlockCompressionTask_t::compressed_block_hash)
	  .def("txn_count", &BTCBlockCompressionTask_t::txn_count)
	  .def("short_ids", &BTCBlockCompressionTask_t::short_ids);
}
