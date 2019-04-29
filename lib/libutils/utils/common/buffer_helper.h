#include <iostream>
#include <cstring>
#include <vector>
#include <array>
#include <algorithm>
#include <cstdint>
#include <type_traits>
#include "utils/common/byte_array.h"

#ifndef UTILS_COMMON_BUFFER_HELPER_H_
#define UTILS_COMMON_BUFFER_HELPER_H_

namespace utils {
namespace common {


template <typename ParseType, typename ReturnType, class TBuffer>
size_t get_little_endian_value(
		const TBuffer& buffer,
		ReturnType& out_value,
		size_t offset
		) {
	size_t type_size = sizeof(ParseType);
	if (type_size > sizeof(uint8_t)) {
		out_value = (ReturnType) *(ParseType *) &buffer[offset];
	} else {
		out_value = (ReturnType) buffer[offset];
	}
	return type_size + offset;
}

template <typename ParseType, typename ReturnType, class TBuffer>
size_t get_big_endian_value(
		const TBuffer& buffer,
		ReturnType& out_value,
		size_t offset
		) {
	constexpr size_t type_size = sizeof(ParseType);
	std::array<uint8_t, type_size> val_array;
	memcpy(
			&val_array[0],
			&buffer[offset],
			type_size
	);
	std::reverse(val_array.begin(), val_array.end());
	return offset + get_little_endian_value<ParseType, ReturnType> (
			val_array,
			out_value,
			0
	);
}

template <typename T, class TBuffer>
size_t set_little_endian_value(
		TBuffer& buffer,
		T value,
		size_t offset
		) {
	constexpr size_t type_size = sizeof(T);
	size_t end = offset + type_size;
	if (buffer.size() < end) {
		buffer.resize(end);
	}

	memcpy((T*)&buffer[offset], &value, type_size);

	return end;
}


} // common
} // utils


#endif /* UTILS_COMMON_BUFFER_HELPER_H_ */
