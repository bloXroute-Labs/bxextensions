#include <iostream>

#include "utils/protocols/ethereum/eth_block_message.h"

#ifndef UTILS_PROTOCOLS_ETHEREUM_BX_ETH_BLOCK_MESSAGE_H_
#define UTILS_PROTOCOLS_ETHEREUM_BX_ETH_BLOCK_MESSAGE_H_

namespace utils {
namespace protocols {
namespace ethereum {

class BxEthBlockMessage {
public:
	BxEthBlockMessage(const common::BufferView& buffer);
    BxEthBlockMessage(BxEthBlockMessage&&) noexcept;
    BxEthBlockMessage& operator =(BxEthBlockMessage&&) noexcept;

	size_t get_next_tx_offset(size_t offset, size_t& from, bool& is_short);
    crypto::Sha256 block_hash() const;
    const common::BufferView& block_header() const;
    const common::BufferView& block_trailer() const;
    size_t txn_end_offset() const;
    size_t txn_offset() const;
    void deserialize_short_ids(std::vector<unsigned int> &short_ids);
	void parse();

private:
	common::BufferView _block_header, _buffer, _block_trailer;
	uint64_t _short_ids_offset;
    crypto::Sha256 _block_hash;
    size_t _txn_offset, _txn_end_offset;
};

} // ethereum
} // protocols
} // utils

#endif /* UTILS_PROTOCOLS_ETHEREUM_BX_ETH_BLOCK_MESSAGE_H_ */
