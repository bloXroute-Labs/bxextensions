#include "utils/common/buffer_view.h"
#include "utils/exception/index_error.h"

namespace utils {
namespace common {

typedef exception::IndexError IndexError_t;


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

BufferView::BufferView(const BufferView& other):
	_buffer(other._buffer),
	_size(other._size),
	_from(other._from)
{
}

BufferView::BufferView(BufferView&& rhs):
			_buffer(rhs._buffer),
			_size(rhs._size),
			_from(rhs._from)
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

BufferView& BufferView::operator=(BufferView&& other) {
	_buffer = other._buffer;
	_from = other._from;
	_size = other._size;
	return *this;
}

BufferView::operator bool() const {
	return _buffer != nullptr;
}

const uint8_t& BufferView::operator[](size_t idx) const {
	return _buffer[_from + idx];
}

const uint8_t& BufferView::at(size_t idx) const {
	if (idx >= _size) {
		throw IndexError_t(idx, _size);
	}
	return this->operator [](idx);
}

const char* BufferView::char_array(void) const {
	return (const char*)&_buffer[_from];
}

const unsigned char* BufferView::byte_array(void) const {
	return &_buffer[_from];
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

BufferView BufferView::empty() {
	static BufferView empty_buffer;
	return empty_buffer;
}

void* BufferView::ptr() {
	return (void *)_buffer;
}

void BufferView::_set_buffer(const uint8_t* buffer, size_t size) {
	_buffer = buffer;
	_size = size;
}

} // common
} // utils
