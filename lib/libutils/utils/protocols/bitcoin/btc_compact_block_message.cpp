#include <utility>
#include <stdint.h>


#include "utils/protocols/bitcoin/btc_compact_block_message.h"
#include "utils/protocols/bitcoin/btc_message_helper.h"
#include "utils/protocols/bitcoin/btc_consts.h"
#include "utils/common/buffer_helper.h"
#include "utils/crypto/hash_helper.h"

namespace utils {
namespace protocols {
namespace bitcoin {


BtcCompactBlockMessage::BtcCompactBlockMessage(
		const common::BufferView& buffer
):
    _buffer(buffer),
    _tx_in_block(buffer)
{
}

BtcCompactBlockMessage::BtcCompactBlockMessage(
		BtcCompactBlockMessage&& rhs
):
    _buffer(rhs._buffer),
    _tx_in_block(rhs._tx_in_block)
{
}

BtcCompactBlockMessage::BtcCompactBlockMessage(
		const BtcCompactBlockMessage& rhs
):
	_buffer(rhs._buffer),
	_tx_in_block(rhs._tx_in_block)
{
}

crypto::Sha256 BtcCompactBlockMessage::block_hash() const
{
	return std::move(generate_block_hash(_buffer));
}

crypto::Sha256 BtcCompactBlockMessage::prev_block_hash() const {
	return std::move(get_prev_block_hash(_buffer));
}

crypto::SipKey_t BtcCompactBlockMessage::get_block_sip_key() const
{
    crypto::Sha256 sha = crypto::Sha256(
		_buffer,
		// TODO convert the following to work with 2 sets of sources
		BTC_HDR_COMMON_OFFSET,
		BTC_BLOCK_HDR_SIZE + BTC_SHORT_NONCE_SIZE

		// TODO: This is the code that should work
        //  {
        //      std::make_pair<size_t, size_t>(BTC_HDR_COMMON_OFFSET, BTC_BLOCK_HDR_SIZE),
        //      std::make_pair<size_t, size_t>(BTC_HDR_COMMON_OFFSET + BTC_BLOCK_HDR_SIZE, BTC_SHORT_NONCE_SIZE)
        //  }
	);

    crypto::SipKey_t sip_key(*(uint64_t *) &sha.binary()[0], *(uint64_t *) &sha.binary()[8]);
    return std::move(sip_key);
}

size_t BtcCompactBlockMessage::get_compact_txs_count(
		uint64_t& tx_count
)
{
	return get_varint(
	        _buffer,
			tx_count,
			BTC_COMPACT_BLOCK_TX_COUNT_OFFSET
	);
}


size_t BtcCompactBlockMessage::get_pre_filled_txs_count(
        size_t offset,
        uint64_t& tx_count
)
{
	return get_varint(
	        _buffer,
			tx_count,
			offset
	);
}

PBufferView_t BtcCompactBlockMessage::get_next_pre_filled_tx(
	    size_t& offset,
		uint64_t& diff,
		Sha256_t& sha,
		int tail/* = -1*/
) const
{
    size_t off = get_varint(
        _buffer,
        diff,
        offset
	);

	// skip the transaction
    offset = _tx_in_block.get_next_tx_offset(
        off,
        tail
    );
    sha = std::move(crypto::double_sha256(_buffer, off, offset - off));
    PBufferView_t tx_content = std::make_shared<BufferView_t>(
    		_buffer, offset - off, off
    );
    return std::move(tx_content);
}

BufferView_t
BtcCompactBlockMessage::get_block_header() const {
	BufferView_t header(
			_buffer, BTC_BLOCK_HDR_SIZE, BTC_HDR_COMMON_OFFSET
	);
	return std::move(header);
}

PCompactShortId_t
BtcCompactBlockMessage::get_next_compact_short_id(size_t& offset) const {
	PCompactShortId_t compact_id = std::make_shared<CompactShortId_t>(
			_buffer,
			offset
	);
	offset += CompactShortId_t::size;
	return std::move(compact_id);
}

const uint32_t BtcCompactBlockMessage::null_tx_sid = NULL_TX_SID;

} // bitcoin
} // protocols
} // utils
