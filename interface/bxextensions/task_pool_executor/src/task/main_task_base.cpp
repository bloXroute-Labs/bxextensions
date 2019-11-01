#include "src/task/main_task_base.h"
#include "src/task/task_base.h"

void bind_main_task_base(py::module& m) {
  py::class_<
    MainTaskBase_t,
    TaskBase_t,
    PMainTaskBase_t>(m, "MainTaskBase")
    .def("cleanup", &MainTaskBase_t::cleanup)
    .def("get_task_byte_size", &MainTaskBase_t::get_task_byte_size);
}
