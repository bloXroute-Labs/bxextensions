#include <iostream>
#include <vector>
#include <cstdint>
#include <functional>

#include "utils/crypto/sha256.h"

#ifndef UTILS_PROTOCOLS_ethereum_ETH_BLOCK_MESSAGE_H_
#define UTILS_PROTOCOLS_ethereum_ETH_BLOCK_MESSAGE_H_

namespace utils {
namespace protocols {
namespace ethereum {

class EthBlockMessage {
public:
    explicit EthBlockMessage(const common::BufferView& buffer);
    EthBlockMessage(EthBlockMessage&&) noexcept;

    EthBlockMessage& operator =(EthBlockMessage&&) noexcept;

    size_t get_next_tx_offset(size_t offset, uint8_t& rlp_type);
    size_t txn_end_offset() const;
    size_t txn_offset() const;
    crypto::Sha256 block_hash() const;
    crypto::Sha256 prev_block_hash() const;

    void parse();

    const common::BufferView& block_header() const;
    const common::BufferView& block_trailer() const;

private:
    common::BufferView _buffer, _block_header, _block_trailer;
    crypto::Sha256 _block_hash, _prev_block_hash;
    size_t _txn_offset, _txn_end_offset;
};

} // ethereum
} // protocols
} // utils

#endif /* UTILS_PROTOCOLS_ethereum_ETH_BLOCK_MESSAGE_H_ */
