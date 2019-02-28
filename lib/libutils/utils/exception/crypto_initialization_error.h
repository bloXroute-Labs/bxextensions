#include <iostream>

#include "utils/common/string_helper.h"
#include "utils/exception/error_base.h"

#ifndef UTILS_EXCEPTION_CRYPTO_INITIALIZATION_ERROR_H_
#define UTILS_EXCEPTION_CRYPTO_INITIALIZATION_ERROR_H_


namespace utils {
namespace exception {

class CryptoInitializationError : public ErrorBase {
  public:
	static const std::string ERROR_TYPE;

	CryptoInitializationError();

    const char* what(void) const noexcept;
};

} // exception
} // utils


#endif /* UTILS_EXCEPTION_CRYPTO_INITIALIZATION_ERROR_H_ */
