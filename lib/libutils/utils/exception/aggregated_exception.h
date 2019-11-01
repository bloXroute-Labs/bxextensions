#include <iostream>
#include <list>
#include "utils/exception/backtrace.h"
#include "utils/common/string_helper.h"
#include "utils/exception/error_base.h"

#ifndef UTILS_EXCEPTION_AGGREGATED_EXCEPTION_H_
#define UTILS_EXCEPTION_AGGREGATED_EXCEPTION_H_

namespace utils {
namespace exception {

class AggregatedException : public ErrorBase {
public:
	static const std::string ERROR_TYPE;

	AggregatedException();
	AggregatedException(const std::exception_ptr& nested);

	const char* what(void) const noexcept;

private:

	std::exception_ptr _nested;
	std::string _error_message;
};

} // exception
} // utils


#endif /* UTILS_EXCEPTION_AGGREGATED_EXCEPTION_H_ */
