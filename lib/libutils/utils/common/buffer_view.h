#include <iostream>
#include <vector>
#include <cstdint>

#ifndef UTILS_COMMON_BUFFER_VIEW_H_
#define UTILS_COMMON_BUFFER_VIEW_H_

namespace utils {
namespace common {

typedef std::vector<uint8_t> Buffer_t;

class BufferView {
	typedef Buffer_t::const_iterator const_iterator_t;
public:
	BufferView(
			const Buffer_t& buffer,
			size_t from = 0,
			size_t length = 0
	);

	BufferView(const BufferView&) = default;

	const_iterator_t begin(void) const;
	const_iterator_t end(void) const;

	size_t size(void) const;

private:
	const Buffer_t& _buffer;
	const size_t _from;
	const size_t _length;
};

} // common
} // utils



#endif /* UTILS_COMMON_BUFFER_VIEW_H_ */
