#include "src/task/decryption_task.h"

#include "src/task/main_task_base.h"


void bind_decryption_task(py::module& m) {
  py::class_<DecryptionTask_t,
   MainTaskBase_t,
   PDecryptionTask_t>(m, "DecryptionTask")
     .def(py::init<size_t>(),
      py::arg("plain_capacity") =
	  PLAIN_TEXT_DEFAULT_BUFFER_SIZE)
     .def("plain",&DecryptionTask_t::plain,
	R"pbdoc(
	  get the plain text.
	  if the decryption process has not yet been completed
	  then wait() will be called.
	  )pbdoc")
    .def("init", &DecryptionTask_t::init);
}
