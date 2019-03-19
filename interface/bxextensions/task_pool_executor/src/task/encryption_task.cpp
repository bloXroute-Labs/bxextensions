#include "src/task/encryption_task.h"

#include "src/task/main_task_base.h"

typedef utils::common::BufferView BufferView_t;

void bind_encryption_task(py::module& m) {
  py::class_<
  EncryptionTask_t,
  MainTaskBase_t,
  PEncryptionTask_t>(m, "EncryptionTask")
      .def(py::init<size_t>(),"initializing",
	    py::arg("plain_capacity") =
		PLAIN_TEXT_DEFAULT_BUFFER_SIZE)
      .def(
    		  "cipher",&EncryptionTask_t::cipher,
			  R"pbdoc(
				get the cipher.
				if the encryption process has not yet been completed
				then wait() will be called.
					)pbdoc",
			  py::return_value_policy::reference

      )
      .def(
    		  "key",
			  &EncryptionTask_t::key,
			  py::return_value_policy::reference
      )
      .def("init", &EncryptionTask_t::init,
	    py::arg("plain"), py::arg("key") = BufferView_t::empty());
}
