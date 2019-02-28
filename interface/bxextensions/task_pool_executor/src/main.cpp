#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
namespace py = pybind11;

#include <memory>
#include <vector>
#include <cstdint>

#include <tpe/task/btc_task_types.h>

PYBIND11_MAKE_OPAQUE(std::vector<uint8_t>);
PYBIND11_MAKE_OPAQUE(task::Sha256ToShortID_t);
PYBIND11_MAKE_OPAQUE(std::vector<utils::crypto::Sha256>);
PYBIND11_MAKE_OPAQUE(std::vector<unsigned int>);
PYBIND11_MAKE_OPAQUE(task::ShortIDToSha256Map_t);
PYBIND11_MAKE_OPAQUE(task::Sha256ToTxMap_t);

#include <tpe/task/test_task.h>
#include <tpe/task/w2_task.h>

#include <utils/crypto/encryption_helper.h>

#include "src/errors.h"
#include "src/tasks.h"
#include "src/byte_array.h"


/**
 * Initializing the task_pool_executor module
 * Python interface definition section
 * method definition - m.def("[method name]", [method function pointer or functor])
 * class definition - py::class_<TClass>(m, "[Python class name]").def(**define public class member function here)
 * binding STL classes:
 * 1. if a collection define it as an OPAQUE (see above) - PYBIND11_MAKE_OPAQUE(std::vector<uint8_t>);
 * 2. bind it inside the interface section - py::bind_vector<std::vector<uint8_t>>(m, "InputBytes");
 */
PYBIND11_MODULE(task_pool_executor, m) {

    // binding custom STL types
    py::bind_vector<std::vector<uint8_t>>(m, "InputBytes");
    py::bind_map<task::Sha256ToShortID_t>(m, "Sha256ToShortIDMap");
    py::bind_vector<std::vector<utils::crypto::Sha256>>(m,"Sha256List");
    py::bind_vector<std::vector<unsigned int>>(m,"UIntList");
    py::bind_map<task::ShortIDToSha256Map_t>(m, "ShortIDToSha256Map");
    py::bind_map<task::Sha256ToTxMap_t>(m, "Sha256ToTxMap");

    bind_byte_array(m);

    // registering errors and binding them to Python error objects
    register_errors(m);

    // calling init for thread pool initialization
    TaskPoolExecutor_t::instance().init();

    // initializing the crypto API
    utils::crypto::init();

    // adding module doc
    m.doc() = R"pbdoc(
        Pybind11 task executor plugin
        -----------------------

        .. currentmodule:: task_pool_executor

        .. autosummary::
           :toctree: _generate

         enqueue_task
    )pbdoc";


    // binding the tasks interface
    bind_tasks(m);

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
