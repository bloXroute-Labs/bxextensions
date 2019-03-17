#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <utils/common/buffer_view.h>

#ifndef SRC_PYTHON_BUFFER_VIEW_H_
#define SRC_PYTHON_BUFFER_VIEW_H_

typedef utils::common::BufferView BufferView_t;

class PythonBufferView : public BufferView_t {
public:
	PythonBufferView(py::buffer buf): _buf(&buf)
	{
		py::buffer_info binfo = std::move(buf.request());
		buf.inc_ref();
		BufferView_t((uint8_t*)binfo.ptr, binfo.size);
	}

	~PythonBufferView() {
		_buf->dec_ref();
	}

private:
	py::buffer *_buf;
};

void bind_python_buffer_view(py::module& m) {
	py::class_<utils::common::BufferView>(m, "BufferView");
	py::class_<PythonBufferView, utils::common::BufferView>(
			m,
			"InputBytes"
	)
	.def(py::init<py::buffer>());
}



#endif /* SRC_PYTHON_BUFFER_VIEW_H_ */
