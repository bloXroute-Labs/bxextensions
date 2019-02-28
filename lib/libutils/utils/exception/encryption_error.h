#include <iostream>

#include "utils/common/string_helper.h"
#include "utils/exception/error_base.h"

#ifndef UTILS_EXCEPTION_ENCRYPTION_ERROR_H_
#define UTILS_EXCEPTION_ENCRYPTION_ERROR_H_

namespace utils {
namespace exception {

class EncryptionError : public ErrorBase {
  public:
	static const std::string ERROR_TYPE;

	EncryptionError();

    const char* what(void) const noexcept;
};

} // exception
} // utils

#endif /* UTILS_EXCEPTION_ENCRYPTION_ERROR_H_ */
