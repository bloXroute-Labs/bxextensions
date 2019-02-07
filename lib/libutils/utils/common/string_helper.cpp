#include <stdio.h>
#include <iomanip>

#include "utils/common/string_helper.h"

namespace utils {
namespace common {

std::string to_hex_string(const std::vector<uint8_t>& bytes) {
	std::ostringstream ss;
	ss << std::hex;
	for (size_t i = 0 ; i < bytes.size() ; ++i) {
		ss << std::setw(2) << std::setfill('0') << (short) bytes[i];
	}
	return ss.str();
}

}
}
