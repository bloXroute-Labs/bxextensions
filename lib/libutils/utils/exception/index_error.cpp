#include "utils/exception/index_error.h"
#include "utils/common/string_helper.h"

namespace utils {
namespace exception {

IndexError::IndexError(size_t idx, size_t size):
	ErrorBase(ERROR_TYPE),
	_idx(idx),
	_size(size)
{
}

IndexError::IndexError():
		ErrorBase(ERROR_TYPE),
		_idx(5),
		_size(4)
{
}

const char* IndexError::what(void) const noexcept {
	   return common::concatenate(
			   "index (", _idx, ") exceeded the array size (",
			    _size, ") .\n",
			   _get_backtrace()
	   ).c_str();
}

const std::string IndexError::ERROR_TYPE = "IndexError";


} // exception
} // utils
