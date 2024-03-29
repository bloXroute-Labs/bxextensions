#include "utils/common/buffer_view.h"
#include "utils/exception/index_error.h"
#include "utils/common/string_helper.h"

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

BufferView::BufferView(
		const BufferView& src, size_t length, size_t from
):
		_buffer(src._buffer),
		_from(src._from + from),
		_size(length)
{
}

BufferView::BufferView(const BufferView& other):
	_buffer(other._buffer),
	_from(other._from),
	_size(other._size)
{
}

BufferView::BufferView(BufferView&& rhs) noexcept:
			_buffer(rhs._buffer),
			_from(rhs._from),
			_size(rhs._size)
{
}

BufferView::~BufferView() = default;

BufferView& BufferView::operator =(const BufferView& other) = default;

BufferView& BufferView::operator =(BufferView&& other) noexcept {
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

const char* BufferView::char_array() const {
	return (const char*)&_buffer[_from];
}

const unsigned char* BufferView::byte_array() const {
	return &_buffer[_from];
}

std::vector<uint8_t> BufferView::vector(size_t offset, size_t length) const {
    return std::vector<uint8_t>(_buffer + offset, _buffer + offset + length);
}

BufferView::const_iterator BufferView::begin() const {
	return const_iterator(_buffer + _from);
}

BufferView::const_iterator BufferView::end() const {
	return const_iterator(_buffer + _from + _size);
};

size_t BufferView::size() const {
	return _size;
}

BufferView BufferView::empty() {
	static BufferView empty_buffer;
	return empty_buffer;
}

void* BufferView::ptr() {
	return (void *)&_buffer[_from];
}

void BufferView::_set_buffer(const uint8_t* buffer, size_t size) {
	_buffer = buffer;
	_size = size;
}

std::ostream& operator <<(std::ostream& out, const BufferView& buf) {
    out << utils::common::to_hex_string(buf.vector(0, buf.size()));
    return out;
}

size_t BufferView::get_offset() const {
    return _from;
};

} // common
} // utils
