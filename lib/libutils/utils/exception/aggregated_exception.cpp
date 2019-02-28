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
}

const char* AggregatedException::what() const noexcept {
	std::ostringstream ss;
	ss << error_type() << ": " << std::endl <<
			_get_backtrace();
	if (_nested) {
		try {
			std::rethrow_exception(_nested);
		} catch (const ErrorBase& e) {
			ss << "is a direct cause of "<< e.error_type()
					<<":"<< e.what() << std::endl;
		} catch (const std::exception& e) {
			ss << "is a direct cause of: " << e.what() << std::endl;
		}

	}
	return ss.str().c_str();
}

const std::string AggregatedException::ERROR_TYPE = "AggregatedException";

} // exception
} // utils
