#include <iostream>

#include "utils/protocols/bitcoin/btc_block_message.h"

#ifndef UTILS_PROTOCOLS_BITCOIN_BX_BTC_BLOCK_MESSAGE_H_
#define UTILS_PROTOCOLS_BITCOIN_BX_BTC_BLOCK_MESSAGE_H_

namespace utils {
namespace protocols {

class BxBtcBlockMessage {
public:
	BxBtcBlockMessage(
			const common::BufferView& buffer, uint64_t short_ids_offset
	);
	size_t get_next_tx_offset(size_t offset, bool& is_short, int tail = -1);
	bool is_short_transaction(size_t offset) const;

	const BTCBlockMessage& block_message(void) const;
	size_t get_tx_count(uint64_t& tx_count);
	void deserialize_short_ids(std::vector<unsigned int>& short_ids);

	static constexpr size_t offset_diff = sizeof(uint64_t);
private:
	common::BufferView _bx_block;
	uint64_t _short_ids_offset;
	BTCBlockMessage _block_message;
};

} // protocols
} // utils

#endif /* UTILS_PROTOCOLS_BITCOIN_BX_BTC_BLOCK_MESSAGE_H_ */
