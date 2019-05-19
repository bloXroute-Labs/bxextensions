#include <iostream>
#include <sstream>
#include <memory>
#include <cstdint>
#include <vector>


#ifndef UTILS_COMMON_STRING_HELPER_H_
#define UTILS_COMMON_STRING_HELPER_H_

namespace utils {
namespace common {

static void add_to_stream(std::ostringstream&) {
	// Do nothing here
}

template<typename T, typename... Args>
static void add_to_stream(std::ostringstream& ss,
		   T&& a_value,
		   Args&&... a_args) {
  ss << std::forward<T>(a_value);
  add_to_stream(ss, std::forward<Args>(a_args)...);
}

template<typename... Args>
std::string concatinate(Args... a_args) {
  std::ostringstream ss;
  add_to_stream(ss, std::forward<Args>(a_args)...);
  return ss.str();
}

std::string to_hex_string(const std::vector<uint8_t>& bytes);
void from_hex_string(
		const std::string& hex_str, std::vector<uint8_t>& bytes
);

} // common
} // utils

#endif /* UTILS_COMMON_STRING_HELPER_H_ */
