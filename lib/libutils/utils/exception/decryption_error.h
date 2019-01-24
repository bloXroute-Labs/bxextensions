#include <iostream>


#ifndef UTILS_EXCEPTION_DECRYPTION_ERROR_H_
#define UTILS_EXCEPTION_DECRYPTION_ERROR_H_

namespace utils {
namespace exception {

class DecryptionError : public std::exception {
public:
  const char* what(void) const noexcept {
    return "Failed to decrypt message.";
  }
};

} // exception
} // utils


#endif /* UTILS_EXCEPTION_DECRYPTION_ERROR_H_ */
