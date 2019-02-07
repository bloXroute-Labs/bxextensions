#include "src/task/bc_compression_task.h"
#include "src/task/main_task_base.h"

#include <utils/crypto/hash_helper.h>

void bind_bc_compression_task(py::module& m) {
	py::class_<utils::crypto::Sha256>(m, "Sha256")
			.def("__repr__", &utils::crypto::Sha256::repr);

	m.def("double_sha256", &utils::crypto::double_sha256);

	py::class_<
	BCCompressionTask_t,
	MainTaskBase_t,
	PBCCompressionTask_t>(m, "BCCompressionTask")
	  .def(py::init<const Sha256ToShortID_t&, size_t>(),
			  "initializing",
			  py::arg("short_id_map"),
			  py::arg("capacity") = BTC_DEFAULT_BLOCK_SIZE)
	  .def("bx_buffer", &BCCompressionTask_t::bx_buffer)
	  .def("init", &BCCompressionTask_t::init);
}
