#include <iostream>
#include <vector>

#include "utils/common/buffer_view.h"

#ifndef UTILS_COMMON_BUFFER_COPY_H_
#define UTILS_COMMON_BUFFER_COPY_H_

namespace utils {
namespace common {

class BufferCopy : public BufferView {
public:
    BufferCopy(const BufferView& buffer);

private:
    std::vector<uint8_t> _buffer_copy;
};

} // common
} // utils

#endif //UTILS_COMMON_BUFFER_COPY_H_
