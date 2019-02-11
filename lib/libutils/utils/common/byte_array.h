#include <iostream>
#include <vector>
#include <cstdint>

#ifndef UTILS_COMMON_BYTE_ARRAY_H_
#define UTILS_COMMON_BYTE_ARRAY_H_

namespace utils {
namespace common {

class ByteArray {
public:
  ByteArray();
  ByteArray(size_t length);
  ByteArray(const ByteArray& other);

  ByteArray& operator=(const ByteArray& other);
  uint8_t& operator[](const size_t idx);
  ByteArray& operator+=(const ByteArray& from);

  const std::vector<unsigned short>& array(void);

  unsigned char* byte_array(void);
  char* char_array(void);

  void reserve(size_t capacity);
  void from_str(const std::string& src, int initial_position = 0);
  void from_char_array(const char *src, size_t length,
		       int initial_position = 0);
  void from_array(const std::vector<uint8_t>& src,
		  size_t initial_position = 0,
		  size_t length = 0);
  size_t copy_from_array(const std::vector<uint8_t>& array,
		  size_t offset,
		  size_t from,
		  size_t length);
  void reset(void);
  void resize(size_t length);
  void clear(void);
  void shift_left(int shift_count);
  void set_output(void);

  size_t size(void) const;
  size_t length(void) const;

private:
  std::vector<uint8_t> _array;
  std::vector<unsigned short> _ret_array;
  size_t _length, _capacity;
};

} // common
} //utils



#endif /* UTILS_COMMON_BYTE_ARRAY_H_ */
