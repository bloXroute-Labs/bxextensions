#include <iostream>

#ifndef UTILS_ENCODING_RLP_ENCODING_TYPE_H
#define UTILS_ENCODING_RLP_ENCODING_TYPE_H

namespace utils {
namespace encoding {

class RlpEncodingType {
public:
    static constexpr uint8_t RLP_STR_ENCODING_TYPE = 128;
    static constexpr uint8_t RLP_LIST_ENCODING_TYPE = 192;
};

} // encoding
} // utils
#endif //UTILS_ENCODING_RLP_ENCODING_TYPE_H
