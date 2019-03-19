#include "src/input_bytes.h"
#include <utils/common/buffer_view.h>

typedef utils::common::BufferView BufferView_t;

class InputBytes : public BufferView_t {
public:
	InputBytes(py::buffer buf);
	~InputBytes();

	py::buffer_info request();
private:
	py::buffer _buf;
};

InputBytes::InputBytes(py::buffer buf) :
	_buf(buf)
{
	buf.inc_ref();
	py::buffer_info binfo = std::move(request());
	_set_buffer((const uint8_t *)binfo.ptr, (size_t)binfo.size);
}

InputBytes::~InputBytes() {
	_buf.dec_ref();
}

py::buffer_info InputBytes::request() {
	return std::move(_buf.request());
}



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
