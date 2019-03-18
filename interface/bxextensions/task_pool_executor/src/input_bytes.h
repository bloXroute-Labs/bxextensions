#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>
#include <vector>

#include <utils/common/buffer_view.h>

#ifndef SRC_PYTHON_BUFFER_VIEW_H_
#define SRC_PYTHON_BUFFER_VIEW_H_

typedef utils::common::BufferView BufferView_t;

class InputBytes : public BufferView_t {
public:
	InputBytes(py::buffer buf): _buf(buf)
	{
		buf.inc_ref();
		py::buffer_info binfo = std::move(buf.request());
		BufferView_t((uint8_t*)binfo.ptr, binfo.size);
	}

	~InputBytes() {
		_buf.dec_ref();
	}

	py::buffer_info request() {
		return std::move(_buf.request());
	}

private:
	py::buffer _buf;
};

void bind_input_bytes(py::module& m) {
	py::class_<BufferView_t>(m, "BufferView");
	py::class_<InputBytes, BufferView_t>(
			m,
			"InputBytes",
			py::buffer_protocol()
	)
	.def(py::init<py::buffer>())
	.def_buffer([](InputBytes& ib) {
		return std::move(ib.request());
	});
}



#endif /* SRC_PYTHON_BUFFER_VIEW_H_ */
