#include <iostream>
#include <vector>
#include <cstdint>
#include <functional>
#include <utility>

#include "utils/crypto/sha256.h"

#ifndef UTILS_PROTOCOLS_BITCOIN_BTC_TX_IN_BLOCK_H_
#define UTILS_PROTOCOLS_BITCOIN_BTC_TX_IN_BLOCK_H_

namespace utils {
namespace protocols {
namespace bitcoin {

class BtcTxInBlock {
public:
	BtcTxInBlock(const common::BufferView& buffer);
	BtcTxInBlock(const BtcTxInBlock& rhs) = default;
	BtcTxInBlock(BtcTxInBlock&& rhs) = default;

	BtcTxInBlock& operator =(const BtcTxInBlock& rhs) = default;
	BtcTxInBlock& operator =(BtcTxInBlock&& rhs) = default;

	size_t get_next_tx_offset(size_t offset, size_t& witness_offset) const;

	bool is_sid_tx(size_t offset);

private:
	size_t _get_tx_io_count_and_offset(
            const std::pair<bool, size_t>& segwit,
			uint64_t& tx_in_count,
			uint64_t& tx_out_count,
			size_t& witness_offset
    ) const;


	common::BufferView _buffer;
};

} // bitcoin
} // protocols
} // utils

#endif /* UTILS_PROTOCOLS_BITCOIN_BTC_TX_IN_BLOCK_H_ */
