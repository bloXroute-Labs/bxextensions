#include "utils/exception/key_error.h"
#include "utils/common/string_helper.h"

namespace utils {
namespace exception {

KeyError::KeyError(const std::string& key):
	ErrorBase(ERROR_TYPE),
	_key(key)
{
}

KeyError::KeyError():
		ErrorBase(ERROR_TYPE),
		_key()
{
}

const char* KeyError::what(void) const noexcept {
	   return common::concatenate(
			   "key (", _key, ") is missing.\n",
			   _get_backtrace()
	   ).c_str();
}

const std::string& KeyError::key() const {
	return _key;
}

const std::string KeyError::ERROR_TYPE = "KeyError";


} // exception
} // utils
