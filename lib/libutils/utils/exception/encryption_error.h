#include <iostream>

#ifndef UTILS_EXCEPTION_ENCRYPTION_ERROR_H_
#define UTILS_EXCEPTION_ENCRYPTION_ERROR_H_

namespace utils {
namespace exception {

class EncryptionError : public std::exception {
  public:
    const char* what(void) const noexcept {
      return "Failed to encrypt message.";
    }
};

} // exception
} // utils

#endif /* UTILS_EXCEPTION_ENCRYPTION_ERROR_H_ */
