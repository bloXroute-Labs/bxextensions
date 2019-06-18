#include <iostream>
#include <vector>
#include <cstdint>
#include <functional>

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

	size_t get_next_tx_offset(size_t offset, int tail = -1) const;

	bool is_sid_tx(size_t offset);

private:
	size_t _get_tx_io_count_and_offset(
			size_t offset,
			uint64_t& txin_count,
			uint64_t& txout_count,
			int tail = -1) const;

	size_t _get_tx_inner_array_offset(
			size_t offset,
			size_t count,
			size_t skip_size,
			int tail = -1,
			std::function<size_t(size_t, size_t)> calc_trail =
					[](size_t offset, size_t count) {
						return offset + count;
			}
	) const;

	common::BufferView _buffer;
};

} // bitcoin
} // protocols
} // utils

#endif /* UTILS_PROTOCOLS_BITCOIN_BTC_TX_IN_BLOCK_H_ */
