#include "utils/common/buffer_view.h"

namespace utils {
namespace common {

BufferView::BufferView() :
	_buffer(nullptr),
	_from(0),
	_size(0)
{
}

BufferView::BufferView(
			const uint8_t* buffer,
			size_t size,
			size_t from/* = 0*/
) :
		_buffer(buffer),
		_from(from),
		_size(size)
{
}

BufferView::BufferView(
		const std::vector<uint8_t>& buffer,
		size_t length/* = 0*/,
		size_t from/* = 0*/
) :
		_buffer(&buffer.at(0)),
		_from(from),
		_size(length ? length: buffer.size())
{
}

BufferView::~BufferView() {
}

BufferView& BufferView::operator=(const BufferView& other) {
	_buffer = other._buffer;
	_from = other._from;
	_size = other._size;
	return *this;
}

const uint8_t& BufferView::operator[](size_t idx) const {
	return _buffer[_from + idx];
}

const uint8_t& BufferView::at(size_t idx) const {
	idx += _from;
	if (idx >= _size) {
		throw std::runtime_error("index out of the array bounds");
	}
	return _buffer[idx];
}

const char* BufferView::char_array(void) const {
	return (const char*)_buffer;
}

const unsigned char* BufferView::byte_array(void) const {
	return _buffer;
}

BufferView::const_iterator BufferView::begin() const {
	return const_iterator(_buffer + _from);
}

BufferView::const_iterator BufferView::end() const {
	return const_iterator(_buffer + _from + _size);
};

size_t BufferView::size(void) const {
	return _size;
}

} // common
} // utils
