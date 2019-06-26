#include <cstring>

#include "buffer_copy.h"

namespace utils {
namespace common {

BufferCopy::BufferCopy(const BufferView& buffer):
    BufferView(),
    _buffer_copy(buffer.size())
{
    memcpy(&_buffer_copy.at(0), &buffer.at(0), buffer.size());
    _set_buffer(&_buffer_copy.at(0), _buffer_copy.size());
}

} // common
} // utils