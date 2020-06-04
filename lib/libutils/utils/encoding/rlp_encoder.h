//
// Created by danielle machpud on 24/05/2020.
//
#include <iostream>
#include "utils/common/buffer_helper.h"
#include "utils/exception/parser_error.h"

#ifndef UTILS_COMMON_RLP_ENCODER_H_
#define UTILS_COMMON_RLP_ENCODER_H_

namespace utils {
namespace encoding {

uint8_t get_length_bytes_size(size_t length);

template <typename T, class TBuffer>
static size_t set_big_endian_rlp_value(TBuffer& buffer, T value, size_t offset, size_t length);

template <typename T, class TBuffer>
size_t rlp_encode(
        TBuffer& buffer,
        T value,
        size_t offset,
        uint8_t encoding_type
)
{
    static constexpr uint8_t length = sizeof(T);
    if (value >= 128) {
        uint8_t length_size = get_length_bytes_size(length);
        // encoding length value
        if (length < 56) {
            offset = utils::common::set_big_endian_value(
                buffer, (uint8_t) (length + encoding_type) , offset
            );
        } else if (value < (1 << 64)) {
            offset = utils::common::set_big_endian_value(
                buffer, (uint8_t) (length_size + 56 - 1 + encoding_type), offset
            );
            offset = set_big_endian_rlp_value(buffer, length, offset, length_size);
        } else {
            // TODO throw exception
        }
    }
    // encoding buffer
    return set_big_endian_rlp_value(buffer, value, offset, length);
}


template <typename T, class TBuffer>
static size_t set_big_endian_rlp_value(
        TBuffer& buffer,
        T value,
        size_t offset,
        size_t length
)
{
    switch (length) {
        case 1:
            offset = utils::common::set_big_endian_value(buffer, (uint8_t) value, offset);
            break;
        case 2:
            offset = utils::common::set_big_endian_value(buffer, (unsigned short) value, offset);
            break;
        case 4:
            offset = utils::common::set_big_endian_value(buffer, (uint32_t) value, offset);
            break;
        case 8:
            offset = utils::common::set_big_endian_value(buffer, (uint64_t) value, offset);
            break;
        default:
            // TODO throw exception
            break;
    }

    return offset;
}


template <class TBuffer>
static size_t get_big_endian_rlp_value(
        TBuffer& buffer,
        uint64_t& value,
        size_t offset,
        size_t length
)
{
    if (length > 4 ) {
       offset = utils::common::get_big_endian_value<uint64_t>(buffer, value, offset, length);
    } else if (length > 2 ) {
        offset = utils::common::get_big_endian_value<uint32_t>(buffer, value, offset, length);
    } else if (length > 1 ) {
        offset = utils::common::get_big_endian_value<unsigned short>(buffer, value, offset, length);
    } else if (length > 0 ) {
        offset = utils::common::get_big_endian_value<uint8_t>(buffer, value, offset, length);
    } else {
        value = 0;
    }

    return offset;
}


template <class TBuffer>
size_t get_length_prefix(const TBuffer& rlp_buf, uint64_t& length, size_t offset) {
    uint8_t b0 = rlp_buf[offset];
    if (b0 < 128) {
        length = 1;
    } else if (b0 < 128 + 56) {
        if ((b0 - 128 == 1) && (rlp_buf[offset + 1] < 128)) {
            throw utils::exception::ParserError();
        }
        length = b0 - 128;
        ++offset;
    } else if (b0 < 192) {
        size_t ll = b0 - 128 - 56 + 1;
        if (rlp_buf[offset + 1] == 0x00) {
            throw utils::exception::ParserError();
        }
        offset = get_big_endian_rlp_value(rlp_buf, length, offset + 1, ll);
        if (length < 56) {
            throw utils::exception::ParserError();
        }
    } else if (b0 < 192 + 56) {
        length = b0 - 192;
        ++offset;
    } else {
        size_t ll = b0 - 192 - 56 + 1;
        if (rlp_buf[offset + 1] == 0x00) {
            throw utils::exception::ParserError();
        }
        offset = get_big_endian_rlp_value(rlp_buf, length, offset + 1, ll);
        if (length < 56) {
            throw utils::exception::ParserError();
        }
    }

    return offset;
}


template <class TBuffer>
size_t rlp_decode(TBuffer& rlp_buf, uint64_t& value, size_t offset){
    uint64_t val_len;
    size_t val_offset = get_length_prefix(rlp_buf, val_len, offset);
    return get_big_endian_rlp_value(rlp_buf, value, val_offset, val_len);
}

} // encoding
} // utils

#endif //UTILS_COMMON_RLP_ENCODER_H_

