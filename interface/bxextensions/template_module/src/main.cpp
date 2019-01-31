#include <pybind11/pybind11.h>
namespace py = pybind11;

// define OPAQUE here



/**
 * Python interface definition section
 * method definition - m.def("[method name]", [method function pointer or functor])
 * class definition - py::class_<TClass>(m, "[Python class name]").def(**define public class member function here)
 * binding STL classes:
 * 1. if a collection define it as an OPAQUE (see above) - PYBIND11_MAKE_OPAQUE(std::vector<uint8_t>);
 * 2. bind it inside the interface section - py::bind_vector<std::vector<uint8_t>>(m, "InputBytes");
 */
PYBIND11_MODULE(test_module, m) {


#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}


