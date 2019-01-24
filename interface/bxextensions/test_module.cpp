#include <pybind11/pybind11.h>
namespace py = pybind11;


PYBIND11_MODULE(test_module, m) {


#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}


