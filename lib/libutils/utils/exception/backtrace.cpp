#include <algorithm>
#if !defined(_AIX) && !(defined(__linux__) && !defined(__GLIBC__))
	#include <execinfo.h>  // for backtrace
	#include <dlfcn.h>     // for dladdr
	#include <cxxabi.h>    // for __cxa_demangle
#endif

#include <string>
#include <sstream>
#include "utils/exception/backtrace.h"

namespace utils {
namespace exception {

void Backtrace::set_backtrace(int skip/* = 1*/) noexcept {
#if !defined(_AIX) && !(defined(__linux__) && !defined(__GLIBC__))
	int nFrames = backtrace(_callstack, MAX_FRAMES);
	char **symbols = backtrace_symbols(_callstack, nFrames);

	std::ostringstream trace_buf;
	for (int i = skip; i < nFrames; i++) {
		Dl_info info;
		if (dladdr(_callstack[i], &info)) {
			char *demangled = NULL;
			int status;
			demangled = abi::__cxa_demangle(info.dli_sname, NULL, 0, &status);
			snprintf(_buf, FRAME_BUFFER_SIZE, "%-3d %*0p %s + %zd\n",
					 i, 2 + sizeof(void*) * 2, _callstack[i],
					 status == 0 ? demangled : info.dli_sname,
					 (char *)_callstack[i] - (char *)info.dli_saddr);
			free(demangled);
		} else {
			snprintf(_buf, FRAME_BUFFER_SIZE, "%-3d %*0p\n",
					 i, 2 + sizeof(void*) * 2, _callstack[i]);
		}
		if (i == skip) {
			_set_separator(std::string("-=begin stack trace=-"), _start);
			trace_buf << _start << std::endl;
		}
		trace_buf << _buf;

		snprintf(_buf, FRAME_BUFFER_SIZE, "%s\n", symbols[i]);
		trace_buf << _buf;
	}
	free(symbols);
	if (nFrames == MAX_FRAMES) {
		std::string truncated;
		_set_separator(std::string("-=truncated=-"), truncated);
		trace_buf << truncated << std::endl;
	}
	_set_separator(std::string("-=end stack trace=-"), _end);
	trace_buf << _end << std::endl;

	_backtrace = trace_buf.str();
#endif
}

const std::string& Backtrace::get_backtrace(void) const {
	return _backtrace;
}

void Backtrace::_set_separator(
		const std::string& title,
		std::string& separator
)
{
	size_t title_size = title.size();
	size_t line_size = std::max(strlen(_buf), title_size);
	separator.resize(line_size, '*');
	size_t offset = (line_size - title_size) / 2;
	std::copy_n(title.begin(), title_size, separator.begin() + offset);
}


} // exception
} // utils
