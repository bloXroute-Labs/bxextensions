#include <iostream>

#ifndef UTILS_EXCEPTION_BACKTRACE_H_
#define UTILS_EXCEPTION_BACKTRACE_H_

namespace utils {
namespace exception {

class Backtrace {
public:
	static constexpr size_t MAX_FRAMES = 128;
	static constexpr size_t FRAME_BUFFER_SIZE = 1024;

	void set_backtrace(int skip = 1) noexcept;

	const std::string& get_backtrace(void) const;



private:
	void _set_separator(
			const std::string& title,
			std::string& separator
	);

	void *_callstack[MAX_FRAMES];
	char _buf[FRAME_BUFFER_SIZE];
	std::string _backtrace;
	std::string _start, _end;

};

} // exception
} // utils



#endif /* UTILS_EXCEPTION_BACKTRACE_H_ */
