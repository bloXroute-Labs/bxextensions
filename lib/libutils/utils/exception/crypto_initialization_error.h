#include <iostream>

#ifndef UTILS_EXCEPTION_CRYPTO_INITIALIZATION_ERROR_H_
#define UTILS_EXCEPTION_CRYPTO_INITIALIZATION_ERROR_H_


namespace utils {
namespace exception {

class CryptoInitializationError : public std::exception {
  public:
    const char* what(void) const noexcept {
      return "Failed initialize libsodium dependency.";
    }
};

} // exception
} // utils


#endif /* UTILS_EXCEPTION_CRYPTO_INITIALIZATION_ERROR_H_ */
