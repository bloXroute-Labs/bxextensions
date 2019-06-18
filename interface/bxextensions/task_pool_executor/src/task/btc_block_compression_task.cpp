#include "src/task/main_task_base.h"
#include "src/task/btc_block_compression_task.h"

#include <utils/crypto/hash_helper.h>

void bind_btc_block_compression_task(py::module& m) {
	py::class_<Sha256_t, PSha256_t>(m, "Sha256")
			.def(py::init<const utils::common::BufferView&>())
			.def("__repr__", &utils::crypto::Sha256::repr)
			.def(
					"binary",
					 &utils::crypto::Sha256::sha256
			)
			.def("__len__", &utils::crypto::Sha256::size)
			.def("hex_string", &utils::crypto::Sha256::hex_string);

	m.def("double_sha256", [](const utils::common::BufferView& buf) {
		return utils::crypto::double_sha256(buf, 0, buf.size());
	});

	py::class_<
	BtcBlockCompressionTask_t,
	MainTaskBase_t,
	PBtcBlockCompressionTask_t>(m, "BtcBlockCompressionTask")
	  .def(py::init<size_t, size_t>(),
			  "initializing",
			  py::arg("capacity") = BTC_DEFAULT_BLOCK_SIZE,
			  py::arg("minimal_tx_count") = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT)
	  .def(
			  "bx_block",
			  &BtcBlockCompressionTask_t::bx_block,
			  py::return_value_policy::reference
	  )
	  .def("init", &BtcBlockCompressionTask_t::init)
	  .def("prev_block_hash", &BtcBlockCompressionTask_t::prev_block_hash)
	  .def("block_hash", &BtcBlockCompressionTask_t::block_hash)
	  .def("compressed_block_hash", &BtcBlockCompressionTask_t::compressed_block_hash)
	  .def("txn_count", &BtcBlockCompressionTask_t::txn_count)
	  .def(
			  "short_ids",
			  &BtcBlockCompressionTask_t::short_ids
	  );
}
