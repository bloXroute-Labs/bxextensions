#include "utils/protocols/internal/block_confirmation_message.h"
#include "utils/common/buffer_helper.h"

#define HEADER_LENGTH 20
#define NETWORK_NUM_LEN sizeof(uint32_t)
#define NODE_ID_LENGTH 16

namespace utils {
namespace protocols {
namespace internal {

BlockConfirmationMessage::BlockConfirmationMessage(const BufferView_t &msg) : _msg(msg) {

}

size_t BlockConfirmationMessage::parse_block_hash(Sha256_t &out_sha) {
    size_t offset = HEADER_LENGTH;
    out_sha = std::move(Sha256_t(_msg, offset));
    return offset + NETWORK_NUM_LEN + SHA256_BINARY_SIZE + NODE_ID_LENGTH;
}

size_t BlockConfirmationMessage::parse_short_ids_count(size_t offset, uint32_t &out_short_id_count) {
    return common::get_little_endian_value<uint32_t>(_msg, out_short_id_count, offset);
}

size_t BlockConfirmationMessage::parse_next_short_id(size_t offset, uint32_t& out_short_id) {
    return common::get_little_endian_value<uint32_t>(_msg, out_short_id, offset);
}

size_t BlockConfirmationMessage::parse_tx_hash_count(size_t offset, uint32_t &out_tx_hash_count) {
    return common::get_little_endian_value<uint32_t>(_msg, out_tx_hash_count, offset);
}

size_t BlockConfirmationMessage::parse_next_tx_hash(size_t offset, Sha256_t &out_sha) {
    out_sha = std::move(Sha256_t(_msg, offset));
    return offset + SHA256_BINARY_SIZE;
}

} // internal
} // protocols
} // utils