#include <iostream>
#include <vector>
#include <cstdint>

#include "utils/common/buffer_view.h"

#ifndef UTILS_COMMON_BYTE_ARRAY_H_
#define UTILS_COMMON_BYTE_ARRAY_H_

namespace utils {
namespace common {

class ByteArray {
public:
  ByteArray();

  explicit ByteArray(size_t capacity);
  ByteArray(const ByteArray& other) = delete;
  ByteArray(ByteArray&& rhs) noexcept;
  ~ByteArray();

  ByteArray& operator=(const ByteArray& other) = delete;
  ByteArray& operator =(ByteArray&& rhs) noexcept;
  uint8_t& operator [](size_t idx);
  ByteArray& operator+=(const ByteArray& from);
  ByteArray& operator+=(const BufferView& from);

  unsigned char* byte_array();
  char* char_array();
  uint8_t& at(size_t idx);
  const uint8_t& at(size_t idx) const;
  const unsigned char* byte_array() const;

    std::vector<uint8_t>* transfer_ownership();

  void reserve(size_t capacity);
  void from_str(const std::string& src, int initial_position = 0);
  void from_char_array(
		  const char *src,
		  size_t length,
		  int initial_position = 0
  );
  void from_array(const std::vector<uint8_t>& src,
		  size_t initial_position = 0,
		  size_t length = 0);
  size_t copy_from_array(const std::vector<uint8_t>& array,
		  size_t offset,
		  size_t from,
		  size_t length);
  size_t copy_from_buffer(const BufferView& buffer,
		  size_t offset,
		  size_t from,
		  size_t length);
  void reset();
  void resize(size_t length);
  void clear();
  void shift_left(int shift_count);

  void set_output();

  size_t size() const;
  size_t length() const;
  size_t capacity() const;
  const std::vector<ssize_t>& shape() const;
  static const std::vector<ssize_t>& strides();
  const std::vector<uint8_t>& array() const;

private:
  std::vector<uint8_t> *_array;
  std::vector<ssize_t> _shape;
  size_t _length, _capacity;
  volatile bool _has_ownership;
};

} // common
} //utils



#endif /* UTILS_COMMON_BYTE_ARRAY_H_ */
