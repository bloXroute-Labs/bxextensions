#include "src/tasks/task_base.h"
#include "src/tasks/encryption_task.h"

void bind_encryption_task(py::module& m) {
  py::class_<
  EncryptionTask_t,
  TaskBase_t,
  PEncryptionTask_t>(m, "EncryptionTask")
      .def(py::init<unsigned long long>(),"initializing",
	    py::arg("plain_capacity") =
		PLAIN_TEXT_DEFAULT_BUFFER_SIZE)
      .def("cipher",[](EncryptionTask_t& tsk) {
	     return tsk.cipher();
	   },
	      R"pbdoc(
		get the cipher.
		if the encryption process has not yet been completed
		then wait() will be called.
	    	)pbdoc")
      .def("key", &EncryptionTask_t::key)
      .def("init", &EncryptionTask_t::init,
	    py::arg("plain"), py::arg("key") = nullptr);
}
