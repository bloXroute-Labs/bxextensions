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
  ByteArray(unsigned long long length);
  ByteArray(const ByteArray& other);

  ByteArray& operator=(const ByteArray& other);

  const std::vector<short>& array(void);

  unsigned char* byte_array(void);
  char* char_array(void);
  unsigned long long length(void);
  void reserve(unsigned long long capacity);
  void from_str(const std::string& src, int initial_position = 0);
  void from_char_array(const char *src, size_t length,
		       int initial_position = 0);
  void from_array(const std::vector<uint8_t>& src,
		  int initial_position = 0);
  void reset(void);
  void resize(unsigned long long length);
  void clear(void);
  void shift_left(int shift_count);

private:
  std::vector<uint8_t> _array;
  std::vector<short> _ret_array;
  unsigned long long _length, _capacity;
};

} // common
} //utils



#endif /* UTILS_COMMON_BYTE_ARRAY_H_ */
