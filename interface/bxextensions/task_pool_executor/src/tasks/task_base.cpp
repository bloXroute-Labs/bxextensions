#include "src/tasks/task_base.h"

#include <memory>

void bind_task_base(py::module& m) {
  py::class_<TaskBase_t, PTaskBase_t>(m, "TaskBase")
     .def("is_completed", &TaskBase_t::is_completed)
     .def("wait",
	  &TaskBase_t::wait,
	  R"pbdoc(
	    wait until task is completed.
	    raise any error that occurred during execution.
	    
	    raise: RuntimeError if task was not yet scheduled for exeuction.
	  )pbdoc")
     .def("get_id",
	  &TaskBase_t::get_id,
	  R"pbdoc(get a unique task identifier)pbdoc");
}
