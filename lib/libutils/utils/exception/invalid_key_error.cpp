#include "utils/exception/invalid_key_error.h"

namespace utils {
namespace exception {

InvalidKeyError::InvalidKeyError(const char* key, int expected_length):
	ErrorBase(ERROR_TYPE),
	_key(key),
	_expected_length(expected_length)
{
}

InvalidKeyError::InvalidKeyError():
		ErrorBase(ERROR_TYPE),
		_key("test key"),
		_expected_length(32)
{
}

const char* InvalidKeyError::what(void) const noexcept {
	   return common::concatenate(
			   "invalid key (", _key, ") length - ",
			   "got ", _key.length(), " expecting ",
			   _expected_length, ".\n",
			   _get_backtrace()
	   ).c_str();
}

const std::string InvalidKeyError::ERROR_TYPE = "InvalidKeyError";


} // exception
} // utils
