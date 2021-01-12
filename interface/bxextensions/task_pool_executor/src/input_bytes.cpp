#include "src/input_bytes.h"
#include <memory>
#include <utils/common/buffer_view.h>

typedef utils::common::BufferView BufferView_t;
typedef std::shared_ptr<BufferView_t> PBufferView_t;

class InputBytes : public BufferView_t {
public:
	InputBytes(py::buffer buf);

	py::buffer_info request();
private:
	py::buffer _buf;
};

typedef std::shared_ptr<InputBytes> PInputBytes;

InputBytes::InputBytes(py::buffer buf) :
	_buf(std::move(buf))
{
	py::buffer_info binfo = request();
	_set_buffer((uint8_t*)binfo.ptr, (size_t)binfo.size);
}

py::buffer_info InputBytes::request() {
	return _buf.request();
}


void bind_input_bytes(py::module& m) {
	py::class_<BufferView_t, PBufferView_t>(m, "BufferView", py::buffer_protocol())
		.def_buffer([](BufferView_t& bv) {
			return py::buffer_info(
				bv.ptr(),
				sizeof(unsigned char),
				py::format_descriptor<uint8_t>::format(),
				1,
				{bv.size()},
				{sizeof(unsigned char)}
			);
		}
	);
	py::class_<InputBytes, BufferView_t, PInputBytes>(
        m,
        "InputBytes",
        py::buffer_protocol()
	)
	.def(py::init<py::buffer>())
	.def_buffer([](InputBytes& ib) {
		return ib.request();
	});
}
