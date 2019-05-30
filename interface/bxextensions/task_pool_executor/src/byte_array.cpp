#include "src/byte_array.h"
#include <utils/common/byte_array.h>
#include <vector>

typedef utils::common::ByteArray ByteArray_t;
typedef std::shared_ptr<ByteArray_t> PByteArray_t;

void bind_byte_array(py::module& m) {
	py::class_<ByteArray_t, PByteArray_t>(
			m,
			"ByteArray",
			py::buffer_protocol()
	)
	.def("__len__", &ByteArray_t::size)
	.def(
			"__getitem__",
			[](ByteArray_t& ba, size_t idx) {return ba.at(idx);}
	)
	.def("__bool__", [](ByteArray_t& ba) {return ba.size() > 0;})
	.def_buffer([](ByteArray_t& ba) {
		return py::buffer_info(
			ba.byte_array(),
			sizeof(unsigned char),
			py::format_descriptor<uint8_t>::format(),
			1,
			ba.shape(),
			ByteArray_t::strides()
		);
	});
}
