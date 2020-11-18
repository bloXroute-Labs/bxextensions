#include <iostream>
#include "utils/protocols/bitcoin/btc_message_helper.h"
#include "utils/common/string_helper.h"
#include "ont_consts.h"

#ifndef UTILS_PROTOCOLS_ONTOLOGY_ONT_MESSAGE_HELPER_H_
#define UTILS_PROTOCOLS_ONTOLOGY_ONT_MESSAGE_HELPER_H_

namespace utils {
namespace protocols {
namespace ontology {

template <class TBuffer>
size_t get_tx_type(const TBuffer& buffer, size_t offset, uint8_t& tx_type) {
    ++offset;
    tx_type = buffer[offset];
    offset += 41; // TODO: make a const
    uint64_t var_size;
    if (tx_type == ONT_TX_DEPLOY_TYPE_INDICATOR) {
        offset = bitcoin::get_varint(buffer, var_size, offset);
        offset += (var_size + 1 /* TODO: make a const*/);
        offset = bitcoin::get_varint(buffer, var_size, offset);
        offset += var_size;
        offset = bitcoin::get_varint(buffer, var_size, offset);
        offset += var_size;
        offset = bitcoin::get_varint(buffer, var_size, offset);
        offset += var_size;
        offset = bitcoin::get_varint(buffer, var_size, offset);
        offset += var_size;
        offset = bitcoin::get_varint(buffer, var_size, offset);
        offset += var_size;
    } else if (tx_type == ONT_TX_INVOKE_TYPE_INDICATOR) {
        offset = bitcoin::get_varint(buffer, var_size, offset);
        offset += var_size;
    }
    return bitcoin::get_varint(buffer, var_size, offset);
}

template <class TBuffer>
crypto::Sha256 get_prev_block_hash(
        const TBuffer& buffer
)
{
    crypto::Sha256 sha(buffer, ONT_PREV_BLOCK_OFF);
    sha.reverse();
    return std::move(sha);
}

template <class TBuffer>
uint32_t get_block_size(const TBuffer& buffer, size_t offset = 0) {
    uint32_t block_size = 0;
    utils::common::get_little_endian_value<uint32_t>(
            buffer,
            block_size,
            offset + ONT_BLOCK_LENGTH_OFFSET
    );
    return block_size + ONT_HDR_COMMON_OFF;
}

template <class TBuffer>
common::BufferView get_merkle_root(
        const TBuffer& buffer
)
{
    size_t offset = get_block_size(buffer) - SHA256_BINARY_SIZE;
    common::BufferView buf(buffer, SHA256_BINARY_SIZE, offset);
    return std::move(buf);
}

template <class TBuffer>
crypto::Sha256 generate_block_hash(
        const TBuffer& buffer,
        size_t header_offset
)
{
    return std::move(
            crypto::double_sha256(
                    buffer,
                    ONT_HDR_COMMON_OFF,
                    header_offset - ONT_HDR_COMMON_OFF
            ));
}

template <class TBuffer>
crypto::Sha256 get_tx_id(const TBuffer& buffer, size_t offset, size_t end) {
    uint8_t tx_type;
    end = get_tx_type(buffer, offset, tx_type);
    return std::move(crypto::double_sha256(buffer, offset, end - offset));
}

} // ontology
} // protocols
} // utils

#endif //UTILS_PROTOCOLS_ONTOLOGY_ONT_MESSAGE_HELPER_H_
