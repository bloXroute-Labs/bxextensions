#include <iostream>
#include "utils/common/buffer_view.h"
#include "utils/crypto/sha256.h"

#ifndef UTILS_PROTOCOLS_INTERNAL_BLOCK_CONFIRMATION_MESSAGE_H
#define UTILS_PROTOCOLS_INTERNAL_BLOCK_CONFIRMATION_MESSAGE_H

namespace utils {
namespace protocols {
namespace internal {

typedef common::BufferView BufferView_t;
typedef crypto::Sha256 Sha256_t;

class BlockConfirmationMessage {
public:
    BlockConfirmationMessage(const BufferView_t& msg);

    size_t parse_block_hash(Sha256_t& out_sha);

    size_t parse_short_ids_count(size_t offset, uint32_t& out_short_id_count);

    size_t parse_next_short_id(size_t offset, uint32_t& out_short_id);

    size_t parse_tx_hash_count(size_t offset, uint32_t& out_tx_hash_count);

    size_t parse_next_tx_hash(size_t offset, Sha256_t& out_sha);

private:
    const BufferView_t& _msg;
};

} // internal
} // protocols
} // utils

#endif //UTILS_PROTOCOLS_INTERNAL_BLOCK_CONFIRMATION_MESSAGE_H
