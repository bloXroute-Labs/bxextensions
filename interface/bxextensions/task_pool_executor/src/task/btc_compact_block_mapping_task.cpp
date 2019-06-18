#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "src/task/main_task_base.h"
#include "src/task/btc_compact_block_mapping_task.h"

typedef task::UnknownTxIndices_t UnknownTxIndecies_t;
typedef std::shared_ptr<UnknownTxIndecies_t> PUnknownTxIndecies_t;

PYBIND11_MAKE_OPAQUE(UnknownTxIndecies_t);

void bind_btc_compact_block_mapping_task(py::module& m) {
	py::bind_vector<UnknownTxIndecies_t, PUnknownTxIndecies_t>(
			m, "UnknownTxIndecies"
	);
	py::class_<
	BtcCompactBlockMappingTask_t,
	MainTaskBase_t,
	PBtcCompactBlockMappingTask_t>(m, "BtcCompactBlockMappingTask")
	  .def(
			  py::init<size_t>(),
			  "initializing",
			  py::arg("capacity") = BTC_DEFAULT_BLOCK_SIZE
	  )
	  .def("init", &BtcCompactBlockMappingTask_t::init)
	  .def("success", &BtcCompactBlockMappingTask_t::success)
	  .def("compression_task", &BtcCompactBlockMappingTask_t::compression_task)
	  .def("missing_indices", &BtcCompactBlockMappingTask_t::missing_indecies);
}
