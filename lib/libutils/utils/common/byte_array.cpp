#include <cstring>
#include <algorithm>

#include "byte_array.h"

namespace utils {
namespace common {

ByteArray::ByteArray():
  _array(),
  _length(0),
  _capacity(0) {

}

ByteArray::ByteArray(size_t capacity):
    _length(0),
    _capacity(capacity) {
  _array.resize(capacity + 1, '\0');
  _ret_array.reserve(capacity);
}

ByteArray::ByteArray(const ByteArray& other):
  _length(other._length),
  _ret_array(other._ret_array),
  _capacity(other._capacity) {
  _array = other._array;
}

ByteArray& ByteArray::operator=(const ByteArray& other) {
  this->_length = other._length;
  this->_array = other._array;
  this->_ret_array = other._ret_array;
  this->_capacity = other._capacity;
  return *this;
}

uint8_t& ByteArray::operator[](const size_t idx) {
	return _array[idx];
}

ByteArray& ByteArray::operator+=(const ByteArray& from) {
	copy_from_array(from._array,
			_length,
			0,
			from.size());
	return *this;
}

unsigned char* ByteArray::byte_array() {
  return &_array[0];
}

char* ByteArray::char_array() {
  return (char *)&_array[0];
}

const std::vector<unsigned short>& ByteArray::array() {
	set_output();
	return _ret_array;
}

size_t ByteArray::length() const {
  return _length;
}

void ByteArray::reserve(size_t capacity) {
  if (_capacity < capacity) {
      _array.resize(capacity + 1, '\0');
      _ret_array.reserve(capacity);
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
  memset(&_array[0],
  		 '\0', sizeof(unsigned char) * (_length + 1));
  _ret_array.clear();
}

void ByteArray::shift_left(int shift_count) {
  if (shift_count > _length) {
      shift_count = _length - 1;
  }
  for (unsigned long long idx = shift_count;
      idx < _length ;
      ++idx) {
      _array[idx - shift_count] = _array[idx];
  }
  _length = _length - shift_count;
}

void ByteArray::set_output() {
	if (_ret_array.size() == 0) {
	  _ret_array.resize(_length);
	  std::copy_n(_array.begin(), _length, _ret_array.begin());
	}
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
	std::copy_n(
			array.begin() + from,
			length,
			_array.begin() + offset
	);
	return total_length;
}

} // common
} // utils
