#include "src/task/main_task_base.h"
#include "src/task/btc_block_compression_task.h"

#include <utils/crypto/hash_helper.h>

void bind_btc_block_compression_task(py::module& m) {
	py::class_<utils::crypto::Sha256>(m, "Sha256")
			.def("__repr__", &utils::crypto::Sha256::repr);

	m.def("double_sha256", &utils::crypto::double_sha256);

	py::class_<
	BTCBlockCompressionTask_t,
	MainTaskBase_t,
	PBTCBlockCompressionTask_t>(m, "BTCBlockCompressionTask")
	  .def(py::init<const task::Sha256ToShortID_t&, size_t, size_t>(),
			  "initializing",
			  py::arg("short_id_map"),
			  py::arg("capacity") = BTC_DEFAULT_BLOCK_SIZE,
			  py::arg("minimal_tx_count") = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT)
	  .def("bx_block", &BTCBlockCompressionTask_t::bx_block)
	  .def("init", &BTCBlockCompressionTask_t::init);
}
