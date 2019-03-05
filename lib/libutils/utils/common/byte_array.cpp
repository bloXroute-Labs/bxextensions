#include <cstring>
#include <algorithm>

#include "byte_array.h"

namespace utils {
namespace common {

ByteArray::ByteArray():
  _length(0),
  _has_ownership(true),
  _capacity(0)
{
	_array = new std::vector<uint8_t>();
	_shape.resize(1);
}

ByteArray::ByteArray(size_t capacity):
    _length(0),
	_has_ownership(true),
    _capacity(capacity)
{
	_array = new std::vector<uint8_t>();
	_array->resize(capacity + 1, '\0');
	_shape.resize(1);
}

ByteArray::ByteArray(ByteArray&& rhs):
	_has_ownership(true)
{
	_capacity = rhs._capacity;
	_array = rhs.transfer_ownership();
	_length = rhs._length;
	_shape = rhs._shape;
}

ByteArray::~ByteArray() {
	if (_has_ownership) {
		delete _array;
	}
	_array = nullptr;
}

ByteArray& ByteArray::operator=(ByteArray&& rhs) {
	_capacity = rhs._capacity;
	_array = rhs.transfer_ownership();
	_length = rhs._length;
	_shape = rhs._shape;
	return *this;
}

uint8_t& ByteArray::operator[](const size_t idx) {
	return _array->at(idx);
}

ByteArray& ByteArray::operator+=(const ByteArray& from) {
	copy_from_array(*from._array,
			_length,
			0,
			from.size());
	return *this;
}

ByteArray& ByteArray::operator+=(const BufferView& from) {
	size_t total_length = _length + from.size(), offset = _length;
	resize(total_length);
	std::copy_n(
			from.begin(),
			from.size(),
			_array->begin() + offset
	);
	return *this;
}

unsigned char* ByteArray::byte_array() {
  return &_array->at(0);
}

char* ByteArray::char_array() {
  return (char *)&_array->at(0);
}

std::vector<uint8_t>* ByteArray::transfer_ownership() {
	_has_ownership = false;
	return _array;
}

size_t ByteArray::length() const {
  return _length;
}

const std::vector<ssize_t>& ByteArray::shape(void) const {
	return _shape;
}

const std::vector<ssize_t>& ByteArray::strides(void) {
	static const std::vector<ssize_t> strides = {
			sizeof(unsigned char)
	};
	return strides;
}

const std::vector<uint8_t>& ByteArray::array(void) const {
	return *_array;
}

void ByteArray::reserve(size_t capacity) {
  if (_capacity < capacity) {
      _array->resize(capacity + 1, '\0');
      _capacity = capacity;
  }
}

void ByteArray::from_str(const std::string& src,
			 int initial_position/* = 0*/) {
  if (src.length() + initial_position > _capacity) {
      reserve(src.length() + initial_position);
  }

  clear();
  memcpy(&char_array()[initial_position],
	  src.c_str(), src.length());
  _length = src.length() + initial_position;
}

void ByteArray::from_char_array(const char *src, size_t length,
		       int initial_position/* = 0*/) {
  if (length + initial_position > _capacity) {
      reserve(length + initial_position);
  }

  clear();
  memcpy(&char_array()[initial_position],
	  src, length);
  _length = length + initial_position;
}

void ByteArray::from_array(const std::vector<uint8_t>& src,
		size_t initial_position/* = 0*/,
		size_t length/* = 0*/) {
  if (length == 0) {
	  length = src.size();
  }
  if (length + initial_position > _capacity) {
      reserve(length + initial_position);
  }

  clear();
  memcpy(&char_array()[initial_position],
	  &src[0], length);
  _length = length + initial_position;
}

void ByteArray::reset() {
  resize(0);
  clear();
}

void ByteArray::resize(size_t length) {
  reserve(length);
  _length = length;
}

void ByteArray::clear() {
  memset(&_array->at(0),
  		 '\0', sizeof(unsigned char) * (_length + 1));
}

void ByteArray::shift_left(int shift_count) {
  if (shift_count > _length) {
      shift_count = _length - 1;
  }
  for (unsigned long long idx = shift_count;
      idx < _length ;
      ++idx) {
      _array->at(idx - shift_count) = _array->at(idx);
  }
  _length = _length - shift_count;
}

size_t ByteArray::extend_from(size_t offset, size_t length) {
	std::vector<uint8_t>::iterator from_it =
			_array->begin() + offset;
	_length += length;
	if (_capacity < _length) {
		_capacity = _length;
	}
	_array->insert(from_it, length, 0);
	return offset + length;
}

void ByteArray::set_output() {
	_shape[0] = size();
}

size_t ByteArray::size() const {
	return _length;
}

size_t ByteArray::copy_from_array(
		const std::vector<uint8_t>& array,
		size_t offset,
		size_t from,
		size_t length
)
{
	size_t total_length = offset + length;
	resize(total_length);
	auto fromIt = array.begin() + from;
	auto targetIt = _array->begin() + offset;
	std::copy_n(
			fromIt,
			length,
			targetIt
	);
	return total_length;
}

} // common
} // utils
