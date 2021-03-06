#include "utils/encoding/rlp_encoder.h"

namespace utils {
namespace encoding {

uint8_t get_length_bytes_size(size_t length) {
    uint8_t shift = 8;
    size_t current_mask = (1 << shift);
    for (uint8_t i = 1; i < 5; ++i) {
        if (length < current_mask) {
            return i;
        } else {
            current_mask = (current_mask << shift);
        }
    }
    return 8;
}

} // encoding
} // utils