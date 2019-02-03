#include "src/task/main_task_base.h"
#include "src/task/task_base.h"

void bind_main_task_base(py::module& m) {
  py::class_<
    MainTaskBase_t,
    TaskBase_t,
    PMainTaskBase_t>(m, "MainTaskBase");
}
