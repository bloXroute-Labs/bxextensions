#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#ifndef SRC_BYTE_ARRAY_H_
#define SRC_BYTE_ARRAY_H_

void bind_byte_array(py::module& m);

#endif /* SRC_BYTE_ARRAY_H_ */
