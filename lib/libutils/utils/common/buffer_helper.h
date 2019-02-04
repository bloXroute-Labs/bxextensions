#include <iostream>
#include <vector>
#include <cstdint>

#ifndef UTILS_COMMON_BUFFER_HELPER_H_
#define UTILS_COMMON_BUFFER_HELPER_H_

namespace utils {
namespace common {

inline unsigned long int get_unsigned_long_int(
		const std::vector<uint8_t>& buffer,
		size_t& offset,
		size_t type_offset) {
	uint8_t typ = buffer[type_offset];
	unsigned long int val;
	switch (typ) {
		case 0xff:
			val = *(unsigned long int *) &buffer[offset];
			offset += sizeof(unsigned long int);
			break;

		case 0xfe:
			val = *(unsigned int *) &buffer[offset];
			offset += sizeof(unsigned int);
			break;

		case 0xfd:
			val = *(unsigned short *) &buffer[offset];
			offset += sizeof(unsigned short);
			break;

		default:
			val = buffer[offset + 1];
			offset += sizeof(uint8_t);
			break;
	}
	return val;
}

} // common
} // utils


#endif /* UTILS_COMMON_BUFFER_HELPER_H_ */
