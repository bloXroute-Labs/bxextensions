#include <iostream>
#include "utils/encoding/rlp_encoding_type.h"
#include <utils/common/buffer_helper.h>
#include <utils/exception/parser_error.h>
#include <utils/protocols/ethereum/eth_consts.h>

#ifndef UTILS_COMMON_RLP_ENCODER_H
#define UTILS_COMMON_RLP_ENCODER_H


namespace utils {
namespace encoding {

typedef utils::encoding::RlpEncodingType RlpEncodingType_t;

template <typename T, class TBuffer>
size_t get_length_prefix_str(TBuffer& buffer, T value, size_t offset);

uint8_t get_length_bytes_size(size_t length);

template <class TBuffer>
static size_t set_big_endian_rlp_value(
    TBuffer& buffer,
    uint64_t& value,
    size_t offset,
    size_t length
)
{
    if ( length > 8 ) {
        offset = utils::common::set_big_endian_value<__int128>(buffer, value, offset, length);
    } else if (length > 4) {
        offset = utils::common::set_big_endian_value<uint64_t>(buffer, value, offset, length);
    } else if ( length > 2 ) {
        offset = utils::common::set_big_endian_value<uint32_t>(buffer, value, offset, length);
    } else if ( length > 1 ) {
        offset = utils::common::set_big_endian_value<uint16_t>(buffer, value, offset, length);
    } else if ( length > 0 ) {
        offset = utils::common::set_big_endian_value<uint8_t>(buffer, value, offset, length);
    } else {
        value = 0;
    }

    return offset;
}

template <class TBuffer>
size_t encode_int(
        TBuffer& buffer,
        uint64_t& value,
        size_t offset
)
{
    uint8_t length_size = get_length_bytes_size(value);
    if (value >= 128) {
        offset = get_length_prefix_str(buffer, length_size, offset);
    }
    // encoding buffer
    return set_big_endian_rlp_value(buffer, value, offset, length_size);
}

template <class TBuffer>
static size_t get_big_endian_rlp_value(
    TBuffer& buffer,
    uint64_t& value,
    size_t offset,
    size_t length
)
{
    if ( length > 8 ) {
        offset = utils::common::get_big_endian_value<__int128>(buffer, value, offset, length);
    } else if ( length > 4 ) {
        offset = utils::common::get_big_endian_value<uint64_t>(buffer, value, offset, length);
    } else if ( length > 2 ) {
        offset = utils::common::get_big_endian_value<uint32_t>(buffer, value, offset, length);
    } else if ( length > 1 ) {
        offset = utils::common::get_big_endian_value<uint16_t>(buffer, value, offset, length);
    } else if ( length > 0 ) {
        offset = utils::common::get_big_endian_value<uint8_t>(buffer, value, offset, length);
    } else {
        value = 0;
    }


    return offset;
}

template <class TBuffer>
static size_t get_big_endian_rlp_large_value(
    TBuffer& buffer,
    uint64_t& value,
    size_t offset,
    size_t length,
    std::vector<uint64_t>& values
) {
    // handle length that is bigger than 8 - need to separate buffer
    while ( length > sizeof(uint64_t) ) {
        offset = utils::common::get_big_endian_value<uint64_t>(buffer, value, offset, sizeof(uint64_t));
        values.push_back(value);
        length -= sizeof(uint64_t);
    }

    offset = get_big_endian_rlp_value(buffer, value, offset, length);
    values.push_back(value);
    return offset;
}

template <class TBuffer>
size_t consume_length_prefix(const TBuffer& rlp_buf, uint64_t& length, uint8_t& rlp_type, size_t offset) {
    if ( offset >= rlp_buf.size() ) {
        throw utils::exception::ParserError();
    }
    uint8_t b0 = rlp_buf[offset];
    if (b0 < 128) {
        length = 1;
    } else if (b0 < 128 + 56) {
        if ((b0 - 128 == 1) && (rlp_buf[offset + 1] < 128)) {
            throw utils::exception::ParserError();
        }
        length = b0 - 128;
        ++offset;
        rlp_type = RLP_BYTE;
    } else if (b0 < 192) {
        size_t ll = b0 - 128 - 56 + 1;
        if (rlp_buf[offset + 1] == 0x00) {
            throw utils::exception::ParserError();
        }
        offset = get_big_endian_rlp_value(rlp_buf, length, offset + 1, ll);
        if (length < 56) {
            throw utils::exception::ParserError();
        }
        rlp_type = RLP_STRING;
    } else if (b0 < 192 + 56) {
        length = b0 - 192;
        ++offset;
        rlp_type = RLP_STRING;
    } else {
        size_t ll = b0 - 192 - 56 + 1;
        if (rlp_buf[offset + 1] == 0x00) {
            throw utils::exception::ParserError();
        }
        offset = get_big_endian_rlp_value(rlp_buf, length, offset + 1, ll);
        if (length < 56) {
            throw utils::exception::ParserError();
        }
        rlp_type = RLP_LIST;
    }

    return offset;
}


template <class TBuffer>
size_t decode_int(TBuffer& rlp_buf, uint64_t& value, size_t offset){
    uint64_t val_len;
    uint8_t rlp_type;
    size_t val_offset = consume_length_prefix(rlp_buf, val_len, rlp_type, offset);
    return get_big_endian_rlp_value(rlp_buf, value, val_offset, val_len);
}

template <class TBuffer>
size_t decode_int(TBuffer& rlp_buf, uint64_t& value, size_t offset, std::vector<uint64_t>& int_values){
    uint64_t val_len;
    uint8_t rlp_type;
    size_t val_offset = consume_length_prefix(rlp_buf, val_len, rlp_type, offset);
    if ( val_len > sizeof(uint64_t) ) {
        return get_big_endian_rlp_large_value(rlp_buf, value, val_offset, val_len, int_values);
    } else {
        return get_big_endian_rlp_value(rlp_buf, value, val_offset, val_len);
    }
}


template <typename T, class TBuffer>
size_t get_length_prefix(
        TBuffer& buffer,
        T value,
        size_t offset,
        uint8_t encoding_type
)
{
    uint8_t length_size = get_length_bytes_size(value);
    if (value < 56) {
        offset = utils::common::set_big_endian_value(
            buffer, (uint8_t) (encoding_type + value) , offset, length_size
        );
    } else {
        offset = utils::common::set_big_endian_value(
            buffer, (uint8_t) (encoding_type + 56 - 1 + length_size), offset, sizeof(uint8_t)
        );
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
        offset = set_big_endian_rlp_value(buffer, (uint64_t&)value, offset, length_size);
#pragma GCC diagnostic pop
    }

    return offset;
}

template <typename T, class TBuffer>
size_t get_length_prefix_str(
        TBuffer& buffer,
        T value,
        size_t offset
)
{
    return get_length_prefix(buffer, value, offset, RlpEncodingType_t::RLP_STR_ENCODING_TYPE);
}

template <typename T, class TBuffer>
size_t get_length_prefix_list(
    TBuffer& buffer,
    T value,
    size_t offset
)
{
    return get_length_prefix(buffer, value, offset, RlpEncodingType_t::RLP_LIST_ENCODING_TYPE);
}


} // encoding
} // utils

#endif //UTILS_COMMON_RLP_ENCODER_H

