#include <iostream>
#include "utils/exception/backtrace.h"

#ifndef UTILS_EXCEPTION_ERROR_BASE_H_
#define UTILS_EXCEPTION_ERROR_BASE_H_
namespace utils {
namespace exception {

class ErrorBase  : public std::exception {

public:

	const std::string& error_type(void) const {
		return _error_type;
	}

	virtual ~ErrorBase() {}

protected:
	ErrorBase() = delete;
	ErrorBase(const std::string& error_type) :
		_error_type(error_type)
	{
		_backtrace.set_backtrace();
	}

	const std::string& _get_backtrace(void) const {
		return _backtrace.get_backtrace();
	}

private:
	const std::string _error_type = "";
	Backtrace _backtrace;

};

} // exception
} // utils

#endif /* UTILS_EXCEPTION_ERROR_BASE_H_ */
