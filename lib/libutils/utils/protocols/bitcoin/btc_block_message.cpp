#include "utils/protocols/bitcoin/btc_block_message.h"
#include "utils/protocols/bitcoin/btc_message_helper.h"
#include "utils/protocols/bitcoin/btc_consts.h"
#include "utils/common/buffer_helper.h"
#include "utils/crypto/hash_helper.h"

namespace utils {
namespace protocols {
namespace bitcoin {

BtcBlockMessage::BtcBlockMessage(
		const common::BufferView& buffer
) : _buffer(buffer),
    _tx_in_block(buffer)
{
}

BtcBlockMessage::BtcBlockMessage(BtcBlockMessage&& rhs) noexcept:
        _buffer(std::move(rhs._buffer)),
        _tx_in_block(std::move(rhs._tx_in_block))
{
}

BtcBlockMessage& BtcBlockMessage::operator =(BtcBlockMessage&& rhs) noexcept {
    _buffer = std::move(rhs._buffer);
    _tx_in_block = std::move(rhs._tx_in_block);
    return *this;
}

size_t BtcBlockMessage::get_next_tx_offset(
	size_t offset,
    int tail/* = -1*/
)
{
    return _tx_in_block.get_next_tx_offset(
        offset,
        tail
    );
}

size_t BtcBlockMessage::get_tx_count(
		uint64_t& tx_count
)
{
	return get_varint(
	        _buffer,
			tx_count,
			BTC_BLOCK_TX_COUNT_OFFSET
	);
}

crypto::Sha256 BtcBlockMessage::block_hash() const {
	return std::move(generate_block_hash(_buffer));
}

crypto::Sha256 BtcBlockMessage::prev_block_hash() const {
	return std::move(get_prev_block_hash(_buffer));
}

} // bitcoin
} // protocols
} // utils
