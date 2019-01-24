#include <iostream>
#include <sstream>
#include <memory>

#ifndef UTILS_COMMON_STRING_HELPER_H_
#define UTILS_COMMON_STRING_HELPER_H_

namespace utils {
namespace common {

static void add_to_stream(std::ostringstream&) {

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

} // common
} // utils

#endif /* UTILS_COMMON_STRING_HELPER_H_ */
