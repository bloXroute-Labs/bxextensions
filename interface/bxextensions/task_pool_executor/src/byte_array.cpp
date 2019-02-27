#include "src/byte_array.h"
#include <utils/common/byte_array.h>
#include <vector>


void bind_byte_array(py::module& m) {
	py::class_<utils::common::ByteArray>(
			m,
			"ByteArray",
			py::buffer_protocol()
	).def_buffer([](utils::common::ByteArray& ba) {
		return py::buffer_info(
			ba.byte_array(),
			sizeof(unsigned char),
			py::format_descriptor<uint8_t>::format(),
			1,
			ba.shape(),
			utils::common::ByteArray::strides()
		);
	});
}
