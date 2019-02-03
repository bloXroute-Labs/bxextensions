#include "src/task/task_base.h"

#include <memory>

void bind_task_base(py::module& m) {
  py::class_<TaskBase_t, PTaskBase_t>(m, "TaskBase")
     .def("is_completed", &TaskBase_t::is_completed)
     .def("get_id",
	  &TaskBase_t::get_id,
	  R"pbdoc(get a unique task identifier)pbdoc");
}
