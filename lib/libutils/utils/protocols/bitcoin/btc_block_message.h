#include <iostream>
#include <vector>
#include <cstdint>
#include <functional>

#ifndef UTILS_PROTOCOLS_BITCOIN_BTC_BLOCK_MESSAGE_H_
#define UTILS_PROTOCOLS_BITCOIN_BTC_BLOCK_MESSAGE_H_

namespace utils {
namespace protocols {

class BTCBlockMessage {
public:
	BTCBlockMessage(const std::vector<uint8_t>& buffer);

	size_t get_varint(
			uint64_t& out_value,
			size_t offset);

	size_t get_next_tx_offset(size_t offset, int tail = -1);
	size_t get_tx_count(uint64_t& tx_count);

private:
	size_t _get_tx_io_count_and_offset(
			size_t offset,
			uint64_t& txin_count,
			uint64_t& txout_count,
			int tail = -1);

	size_t _get_tx_inner_array_offset(
			size_t offset,
			size_t count,
			size_t skip_size,
			int tail = -1,
			std::function<size_t(size_t, size_t)> calc_trail =
					[](size_t offset, size_t count) {
						return offset + count;
			}
	);

	const std::vector<uint8_t>& _buffer;
};

} // protocols
} // utils

#endif /* UTILS_PROTOCOLS_BITCOIN_BTC_BLOCK_MESSAGE_H_ */
