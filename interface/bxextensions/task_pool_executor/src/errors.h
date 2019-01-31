#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#ifndef SRC_ERRORS_H_
#define SRC_ERRORS_H_


void register_errors(py::module& m);


#endif /* SRC_ERRORS_H_ */
