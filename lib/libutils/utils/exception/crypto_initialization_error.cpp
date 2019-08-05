#include "utils/exception/crypto_initialization_error.h"

namespace utils {
namespace exception {

CryptoInitializationError::CryptoInitializationError():
		ErrorBase(ERROR_TYPE)
{
}

const char* CryptoInitializationError::what(void) const noexcept  {
	return common::concatenate(
		  "Failed initialize libsodium dependency\n",
		  _get_backtrace()
	).c_str();
}

const std::string CryptoInitializationError::ERROR_TYPE = "CryptoInitializationError";


} // exception
} // utils
