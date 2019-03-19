#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>


#ifndef SRC_PYTHON_BUFFER_VIEW_H_
#define SRC_PYTHON_BUFFER_VIEW_H_

void bind_input_bytes(py::module& m);

#endif /* SRC_PYTHON_BUFFER_VIEW_H_ */
