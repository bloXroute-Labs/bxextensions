#include <iostream>

#include "utils/common/string_helper.h"
#include "utils/exception/error_base.h"


#ifndef UTILS_EXCEPTION_INVALID_KEY_ERROR_H_
#define UTILS_EXCEPTION_INVALID_KEY_ERROR_H_

namespace utils {
namespace exception {

class InvalidKeyError : ErrorBase {
public:
  static const std::string ERROR_TYPE;

  InvalidKeyError(const char* key, int expected_length);

  const char* what(void) const noexcept;

private:
  std::string _key;
  int _expected_length;
};

} // exception
} // utils


#endif /* UTILS_EXCEPTION_INVALID_KEY_ERROR_H_ */
