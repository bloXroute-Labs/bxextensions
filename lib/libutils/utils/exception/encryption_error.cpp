#include "utils/exception/encryption_error.h"

namespace utils {
namespace exception {

EncryptionError::EncryptionError():
		ErrorBase(ERROR_TYPE)
{
}

const char* EncryptionError::what(void) const noexcept {
	return common::concatenate(
		  "Failed to encrypt message\n",
		  _get_backtrace()
	).c_str();
}

const std::string EncryptionError::ERROR_TYPE = "EncryptionError";


} // exception
} // utils
