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

ByteArray::ByteArray(const std::string& src):
  _length(src.length()),
  _str(src),
  _capacity(src.length()) {
  _array.resize(_length + 1, '\0');
  strcpy(char_array(), src.c_str());
}

ByteArray& ByteArray::operator= (const ByteArray& other) {
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
      _str = std::string(char_array());
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

void ByteArray::from_str(const std::string& src) {
  if (src.length() > _capacity) {
      reserve(src.length());
      _length = src.length();
  } else {
      strcpy(char_array(), src.c_str());
      _length = src.length();
      _str = src;
  }
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

} // common
} // utils
