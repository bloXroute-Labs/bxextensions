#include "utils/common/string_helper.h"
#include "utils/exception/aggregated_exception.h"

namespace utils {
namespace exception {

AggregatedException::AggregatedException():
		ErrorBase(ERROR_TYPE),
		_nested(nullptr)
{
}

AggregatedException::AggregatedException(
		const std::exception_ptr& nested

):
		ErrorBase(ERROR_TYPE),
		_nested(nested)
{
	_error_message = utils::common::concatenate(
		error_type(),
		": \n",
		_get_backtrace()
	);
	if (_nested) {
		try {
			std::rethrow_exception(_nested);
		} catch (const ErrorBase& e) {
			_error_message = utils::common::concatenate(
				_error_message,
				"is a direct cause of ",
				e.error_type(),
				": ",
				e.what(),
				"\n"
			); 
		} catch (const std::exception& e) {
			_error_message = utils::common::concatenate(
				_error_message,
				"is a direct cause of: ",
				e.what(),
				"\n"
			); 
		}
	}
}

const char* AggregatedException::what() const noexcept {
	return _error_message.c_str();
}

const std::string AggregatedException::ERROR_TYPE = "AggregatedException";

} // exception
} // utils
