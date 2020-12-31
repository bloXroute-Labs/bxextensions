#include <iostream>
#include <sstream>
#include <memory>
#include <cstdint>
#include <vector>

#include <stdio.h>
#include <iomanip>


#ifndef UTILS_COMMON_STRING_HELPER_H_
#define UTILS_COMMON_STRING_HELPER_H_

namespace utils {
namespace common {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static void add_to_stream(std::ostringstream&) {
	// Do nothing here
}
#pragma GCC diagnostic pop

template<typename T, typename... Args>
static void add_to_stream(std::ostringstream& ss,
		   T&& a_value,
		   Args&&... a_args) {
  ss << std::forward<T>(a_value);
  add_to_stream(ss, std::forward<Args>(a_args)...);
}

template<typename... Args>
std::string concatenate(Args... a_args) {
  std::ostringstream ss;
  add_to_stream(ss, std::forward<Args>(a_args)...);
  return ss.str();
}

template <class TBytes>
std::string to_hex_string(
    const TBytes& bytes
)
{
    std::ostringstream ss;
    ss << std::hex;
    for (unsigned char byte : bytes) {
        ss << std::setw(2) << std::setfill('0') << (short) byte;
    }
    return ss.str();
}

void from_hex_string(
    const std::string& hex_str,
    std::vector<uint8_t>& bytes
);

} // common
} // utils

#endif /* UTILS_COMMON_STRING_HELPER_H_ */
