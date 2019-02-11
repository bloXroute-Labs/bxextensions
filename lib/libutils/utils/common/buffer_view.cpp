#include "utils/common/buffer_view.h"

namespace utils {
namespace common {

BufferView::BufferView(
			const Buffer_t& buffer,
			size_t from/* = 0*/,
			size_t length/* = 0*/
) :
		_buffer(buffer),
		_from(from),
		_length(length ? length : buffer.size() - from)
{
}

BufferView::const_iterator_t BufferView::begin() const {
	return _buffer.begin() + _from;
}

BufferView::const_iterator_t BufferView::end() const {
	return _buffer.begin() + _from + _length;
};

size_t BufferView::size(void) const {
	return _length;
}

} // common
} // utils
