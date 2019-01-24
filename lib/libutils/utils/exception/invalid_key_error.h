#include <iostream>

#include "utils/common/string_helper.h"


#ifndef UTILS_EXCEPTION_INVALID_KEY_ERROR_H_
#define UTILS_EXCEPTION_INVALID_KEY_ERROR_H_

namespace utils {
namespace exception {

class InvalidKeyError : public std::exception {
public:

  InvalidKeyError(const std::string key, int expected_length):
    std::exception(),
    _key(key),
    _expected_length(expected_length) {

  }

  const char* what(void) const noexcept {
    return common::concatinate(
	"invalid key (", _key, ") length - ",
	"got ", _key.length(), " expecting ",
	_expected_length).c_str();
  }

private:
  std::string _key;
  int _expected_length;
};

} // exception
} // utils


#endif /* UTILS_EXCEPTION_INVALID_KEY_ERROR_H_ */
