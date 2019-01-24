#include <pybind11/pybind11.h>
namespace py = pybind11;

#include <memory>

#include "tpe/task_pool_executor.h"
#include "tpe/task/decryption_task.h"
#include "tpe/task/encryption_task.h"
#include "tpe/task/test_task.h"
#include "tpe/task/exception/task_not_executed.h"

#include "utils/crypto/encrypted_message.h"
#include "utils/crypto/encryption_helper.h"
#include "utils/exception/encryption_error.h"
#include "utils/exception/decryption_error.h"
#include "utils/exception/crypto_initialization_error.h"
#include "utils/exception/invalid_key_error.h"

typedef task::TaskBase TaskBase_t;
typedef task::EncryptionTask EncryptionTask_t;
typedef task::DecryptionTask DecryptionTask_t;
typedef std::shared_ptr<TaskBase_t> PTaskBase_t;
typedef std::shared_ptr<EncryptionTask_t> PEncryptionTask_t;
typedef std::shared_ptr<DecryptionTask_t> PDecryptionTask_t;
typedef task::pool::TaskPoolExecutor TaskPoolExecutor_t;
typedef task::exception::TaskNotExecuted TaskNotExecuted_t;
typedef utils::crypto::EncryptedMessage EncryptedMessage_t;
typedef std::shared_ptr<EncryptedMessage_t> PEncryptedMessage_t;
typedef utils::exception::EncryptionError EncryptionError_t;
typedef utils::exception::DecryptionError DecryptionError_t;
typedef utils::exception::InvalidKeyError InvalidKeyError_t;
typedef utils::exception::CryptoInitializationError CryptoInitializationError_t;

void enqueue_task(PTaskBase_t task) {
  TaskPoolExecutor_t::instance().enqueue_task(task);
}

/**
 * Initializing the task_pool_executor module
 */
PYBIND11_MODULE(task_pool_executor, m) {

    /**
     * custom exception registration section:
     * template - py::registe_exception<TException>(m, "[Python error class name]"
     */
    // -----------------------------------Begin----------------------------------------
    py::register_exception<TaskNotExecuted_t>(m, "TaskNotExecutedError");
    py::register_exception<EncryptionError_t>(m, "EncryptionError");
    py::register_exception<DecryptionError_t>(m, "DecryptionError");
    py::register_exception<InvalidKeyError_t>(m, "InvalidKeyError");
    py::register_exception<CryptoInitializationError_t>(m, "CryptoInitializationError");
    // -----------------------------------End------------------------------------------

    // calling init for thread pool initialization
    // -----------------------------------Begin----------------------------------------
    TaskPoolExecutor_t::instance().init();
    // -----------------------------------End------------------------------------------

    // initializing the crypto API
    // -----------------------------------Begin----------------------------------------
    utils::crypto::init();
    // -----------------------------------End------------------------------------------


    // -----------------------------------Begin----------------------------------------
    m.doc() = R"pbdoc(
        Pybind11 task executor plugin
        -----------------------

        .. currentmodule:: task_pool_executor

        .. autosummary::
           :toctree: _generate

         enqueue_task
    )pbdoc";
    // -----------------------------------End------------------------------------------


  /**
   * Python interface definition section
   * method definition - m.def("[method name]", [method function pointer or functor])
   * class definition - py::class_<TClass>(m, "[Python class name]").def(**define public class member function here)
   */

   // -----------------------------------Begin----------------------------------------

  /**
   * Encrypted message definition
   */
   // -----------------------------------Begin----------------------------------------
   py::class_<EncryptedMessage_t, PEncryptedMessage_t>(
       m, "EncryptedMessage"
   ).
       def(py::init<const std::string&>()).
       def("cipher", [](EncryptedMessage_t& msg) {
	 return py::bytes(msg.cipher());
       }).
       def("cipher_text", [](EncryptedMessage_t& msg) {
	 return py::bytes(msg.cipher_text());
       }).
       def("nonce", [](EncryptedMessage_t& msg) {
	 return py::bytes(msg.nonce());
       });
   // -----------------------------------End------------------------------------------

  /**
   * Task base definition
   */
   // -----------------------------------Begin----------------------------------------
   py::class_<TaskBase_t, PTaskBase_t>(
       m, "TaskBase")
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

   // -----------------------------------End------------------------------------------

   /**
    * Encryption task definition
    */
    // -----------------------------------Begin----------------------------------------
   py::class_<
   EncryptionTask_t,
   TaskBase_t,
   PEncryptionTask_t>(m, "EncryptionTask")
       .def(py::init<unsigned long long>(),"initializing",
	    py::arg("plain_capacity") =
		PLAIN_TEXT_DEFAULT_BUFFER_SIZE)
       .def("cipher",[](EncryptionTask_t& tsk) {
	     return std::make_shared<EncryptedMessage_t>(tsk.cipher());
	   },
	      R"pbdoc(
		get the cipher.
		if the encryption process has not yet been completed
		then wait() will be called.
	    	)pbdoc")
       .def("key", [](EncryptionTask_t& tsk) {
	 return py::bytes(tsk.key());
       })
       .def("init", &EncryptionTask_t::init, py::arg("plain"), py::arg("key")="");
   // -----------------------------------End------------------------------------------

   /**
    * Decryption task definition
    */
    // -----------------------------------Begin----------------------------------------
   py::class_<DecryptionTask_t,
     TaskBase_t,
     PDecryptionTask_t>(m, "DecryptionTask")
       .def(py::init<unsigned long long>(),
	    py::arg("plain_capacity") =
		PLAIN_TEXT_DEFAULT_BUFFER_SIZE)
       .def("plain",[](DecryptionTask_t& tsk) {
	       return py::bytes(tsk.plain());
	     },
	      R"pbdoc(
		get the plain text.
		if the decryption process has not yet been completed
		then wait() will be called.
	    	)pbdoc")
      .def("init", &DecryptionTask_t::init);
   // -----------------------------------End------------------------------------------

   /**
    * Test task definition
    */
    // -----------------------------------Begin----------------------------------------
   py::class_<task::TestTask,
     task::TaskBase,
     std::shared_ptr<task::TestTask>>(m, "TestTask")
       .def(py::init<>())
       .def("t1", &task::TestTask::t1)
       .def("t2", &task::TestTask::t2)
       .def("t3", &task::TestTask::t3)
       .def("init", &task::TestTask::init);
   // -----------------------------------End------------------------------------------


   /**
    * enqueue task definition
    */
    // -----------------------------------Begin----------------------------------------
    m.def("enqueue_task", &enqueue_task, R"pbdoc(
	add task to a thread pool queue
	)pbdoc");
    // -----------------------------------End------------------------------------------


    // -----------------------------------End------------------------------------------

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
