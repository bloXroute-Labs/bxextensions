#include "src/tasks.h"
#include <tpe/task/test_task.h>
#include <tpe/task/w2_task.h>
#include "task/task_base.h"
#include "task/decryption_task.h"
#include "task/encryption_task.h"
#include "task/bc_compression_task.h"


static void bind_test_task(py::module& m) {
  py::class_<task::TestTask,
    task::MainTaskBase,
    std::shared_ptr<task::TestTask>>(m, "TestTask")
      .def(py::init<>())
      .def("t1", &task::TestTask::t1)
      .def("t2", &task::TestTask::t2)
      .def("t3", &task::TestTask::t3)
      .def("init", &task::TestTask::init);
}

static void bind_w2_task(py::module& m) {
  py::class_<task::W2Task,
    task::MainTaskBase,
    std::shared_ptr<task::W2Task>>(m, "W2Task")
      .def(py::init<>())
      .def("init", &task::W2Task::init);
}

void bind_tasks(py::module& m) {
  bind_task_base(m);
  bind_main_task_base(m);
  bind_encryption_task(m);
  bind_decryption_task(m);
  bind_bc_compression_task(m);
  bind_test_task(m);
  bind_w2_task(m);
  m.def("enqueue_task", &enqueue_task, R"pbdoc(
	add task to a thread pool queue
	)pbdoc");
}

void enqueue_task(PMainTaskBase_t task) {
  TaskPoolExecutor_t::instance().enqueue_task(task);
}
