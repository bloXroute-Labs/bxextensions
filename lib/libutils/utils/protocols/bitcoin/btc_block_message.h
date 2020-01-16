#include <iostream>
#include <vector>
#include <cstdint>
#include <functional>

#include "utils/protocols/bitcoin/btc_tx_in_block.h"
#include "utils/crypto/sha256.h"

#ifndef UTILS_PROTOCOLS_BITCOIN_BTC_BLOCK_MESSAGE_H_
#define UTILS_PROTOCOLS_BITCOIN_BTC_BLOCK_MESSAGE_H_

namespace utils {
namespace protocols {
namespace bitcoin {

class BtcBlockMessage {
public:
	explicit BtcBlockMessage(const common::BufferView& buffer);
    BtcBlockMessage(BtcBlockMessage&&) noexcept;

    BtcBlockMessage& operator =(BtcBlockMessage&&) noexcept;

	size_t get_next_tx_offset(size_t offset, size_t& witness_offset);
	size_t get_tx_count(uint64_t& tx_count);

	crypto::Sha256 block_hash() const;
	crypto::Sha256 prev_block_hash() const;

private:
	common::BufferView _buffer;
	BtcTxInBlock _tx_in_block;
};

} // bitcoin
} // protocols
} // utils

#endif /* UTILS_PROTOCOLS_BITCOIN_BTC_BLOCK_MESSAGE_H_ */
