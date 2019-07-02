#include <iostream>
#include <unordered_map>
#include <functional>

#include "utils/common/buffer_helper.h"

namespace utils {
namespace common {
//
//static const std::unordered_map<uint8_t, std::function<size_t(
//		const std::vector<uint8_t>&,
//		unsigned long int &,
//		size_t
//		)>> varint_parsers = {
//				{0xff, get_little_endian_value<unsigned long int, unsigned long int>},
//				{0xfe, get_little_endian_value<unsigned int, unsigned long int>},
//				{0xfd, get_little_endian_value<unsigned short, unsigned long int>}
//		};
//
//unsigned long int get_var_int(
//		const std::vector<uint8_t>& buffer,
//		size_t& offset,
//		size_t type_offset) {
//
//
//}
size_t get_compact_short_id(
		const common::BufferView& buffer,
		uint64_t& out_value,
		const size_t offset
)
{
	constexpr size_t source_size = 6;

	std::array<uint8_t, sizeof(uint64_t)> val_array;
    val_array.fill(0);
	memcpy(
			&val_array[0],
			&buffer[offset],
			source_size
	);
	out_value = *(uint64_t *) &val_array[0];
	return offset + source_size;
}

} // common
} // utils
