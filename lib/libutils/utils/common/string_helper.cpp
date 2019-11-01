#include <stdio.h>
#include <iomanip>

#include "utils/common/string_helper.h"

namespace utils {
namespace common {

void from_hex_string(
		const std::string& hex_str, std::vector<uint8_t>& bytes
)
{
	std::stringstream ss;
	for (size_t idx = 0 ; idx < hex_str.size() ; idx += 2) {
		std::string hex_pair = hex_str.substr(idx, 2);
		unsigned short byte;
		ss << std::hex << hex_pair;
		ss >> std::hex >> byte;
		bytes.push_back(byte);
		ss.clear();
	}

}

}
}
