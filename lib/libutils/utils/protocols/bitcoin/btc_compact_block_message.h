#include <iostream>
#include <vector>
#include <cstdint>
#include <functional>
#include <atomic>
#include <utility>

#include "utils/crypto/hash_helper.h"
#include "utils/crypto/compact_short_id.h"
#include "utils/protocols/bitcoin/btc_tx_in_block.h"


#ifndef UTILS_PROTOCOLS_BITCOIN_BTC_COMPACT_BLOCK_MESSAGE_H_
#define UTILS_PROTOCOLS_BITCOIN_BTC_COMPACT_BLOCK_MESSAGE_H_

namespace utils {
namespace protocols {
namespace bitcoin {

typedef crypto::CompactShortId CompactShortId_t;
typedef std::shared_ptr<CompactShortId_t> PCompactShortId_t;
typedef crypto::Sha256 Sha256_t;
typedef common::BufferView BufferView_t;
typedef std::shared_ptr<BufferView_t> PBufferView_t;

class BtcCompactBlockMessage {
public:
    // TODO implement empty constructor & copy constructor
	BtcCompactBlockMessage(const BufferView_t& buffer);
	BtcCompactBlockMessage(BtcCompactBlockMessage&& rhs);
	BtcCompactBlockMessage(const BtcCompactBlockMessage& rhs);

	size_t get_compact_txs_count(uint64_t& tx_count);
	size_t get_pre_filled_txs_count(
	    size_t offset,
	    uint64_t& tx_count
    );

	PBufferView_t get_next_pre_filled_tx(
	    size_t& offset,
		uint64_t& diff,
		Sha256_t& sha,
	    int tail = -1
    ) const;

	BufferView_t get_block_header(void) const;

	PCompactShortId_t get_next_compact_short_id(size_t& offset) const;

    crypto::Sha256 block_hash(void) const;
    crypto::Sha256 prev_block_hash(void) const;
    crypto::SipKey_t get_block_sip_key() const;

    static const uint32_t null_tx_sid;

private:

	const BufferView_t& _buffer;
	BtcTxInBlock _tx_in_block;
};

} // bitcoin
} // protocols
} // utils

#endif /* UTILS_PROTOCOLS_BITCOIN_BTC_COMPACT_BLOCK_MESSAGE_H_ */
