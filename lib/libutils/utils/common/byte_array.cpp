#include <cstring>

#include "byte_array.h"

namespace utils {
namespace common {

ByteArray::ByteArray():
  _array(),
  _str(),
  _length(0),
  _capacity(0) {

}

ByteArray::ByteArray(unsigned long long length):
    _length(length),
    _str(),
    _capacity(length) {
  _array.resize(length + 1, '\0');
  _str.reserve(length);
}

ByteArray::ByteArray(const ByteArray& other):
  _length(other._length),
  _str(other._str),
  _capacity(other._capacity) {
  _array = other._array;
}

ByteArray& ByteArray::operator=(const ByteArray& other) {
  this->_length = other._length;
  this->_str = other._str;
  this->_array = other._array;
  this->_capacity = other._capacity;
  return *this;
}

unsigned char* ByteArray::byte_array() {
  return &_array[0];
}

char* ByteArray::char_array() {
  return (char *)&_array[0];
}

const std::string& ByteArray::str() {
  if(_str.empty()) {
      _str.assign(char_array(), _length);
  }
  return _str;
}

unsigned long long ByteArray::length() {
  return _length;
}

void ByteArray::reserve(unsigned long long capacity) {
  if (_capacity < capacity) {
      _array.resize(capacity + 1, '\0');
      _str.reserve(capacity);
      _capacity = capacity;
  }
}

void ByteArray::from_str(const std::string& src,
			 int initial_position/* = 0*/) {
  if (src.length() + initial_position > _capacity) {
      reserve(src.length() + initial_position);
  }

  clear();
  strncpy(&char_array()[initial_position],
	  src.c_str(), src.length());
  _length = src.length() + initial_position;
  _str = src;
}

void ByteArray::from_char_array(const char *src, size_t length,
		       int initial_position/* = 0*/) {
  if (length + initial_position > _capacity) {
      reserve(length + initial_position);
  }

  clear();
  strncpy(&char_array()[initial_position],
	  src, length);
  _length = length + initial_position;
  _str = src;
}

void ByteArray::reset() {
  resize(0);
  clear();
}

void ByteArray::resize(unsigned long long length) {
  reserve(length);
  _length = length;
}

void ByteArray::clear() {
  memset(&_array[0],
  		 '\0', sizeof(unsigned char) * (_length + 1));
  _str.clear();
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
  _str.clear();
}

} // common
} // utils
