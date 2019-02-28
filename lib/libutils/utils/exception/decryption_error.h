#include <iostream>

#include "utils/common/string_helper.h"
#include "utils/exception/error_base.h"


#ifndef UTILS_EXCEPTION_DECRYPTION_ERROR_H_
#define UTILS_EXCEPTION_DECRYPTION_ERROR_H_

namespace utils {
namespace exception {

class DecryptionError : ErrorBase {
public:
  static const std::string ERROR_TYPE;

  DecryptionError();

  const char* what(void) const noexcept;
};

} // exception
} // utils


#endif /* UTILS_EXCEPTION_DECRYPTION_ERROR_H_ */
