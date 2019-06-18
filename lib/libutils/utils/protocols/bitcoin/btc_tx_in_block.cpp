#include "utils/protocols/bitcoin/btc_tx_in_block.h"
#include "utils/protocols/bitcoin/btc_block_message.h"
#include "utils/protocols/bitcoin/btc_message_helper.h"
#include "utils/protocols/bitcoin/btc_consts.h"
#include "utils/common/buffer_helper.h"
#include "utils/crypto/hash_helper.h"

namespace utils {
namespace protocols {
namespace bitcoin {

BtcTxInBlock::BtcTxInBlock(
		const common::BufferView& buffer
) : _buffer(buffer)
{
}


size_t BtcTxInBlock::get_next_tx_offset(
	size_t offset,
    int tail/* = -1*/
) const
{
    offset += BTC_TX_VERSION_SIZE;
	uint16_t segwit_flag = 0;
	size_t segwit_offset = common::get_big_endian_value<uint16_t> (
			_buffer,
			segwit_flag,
			offset
	);
	if (segwit_flag == BTC_TX_SEGWIT_FLAG_VALUE) {
		offset = segwit_offset;
	}
	uint64_t txin_count = 0, txout_count = 0;
	offset = _get_tx_io_count_and_offset(
			offset, txin_count, txout_count, tail
	);
	if (segwit_flag == BTC_TX_SEGWIT_FLAG_VALUE) {
		offset = _get_tx_inner_array_offset(
				offset,
				txin_count,
				0,
				tail,
				[&](size_t seg_offset, size_t seg_count) {
					return _get_tx_inner_array_offset(
							seg_offset,
							seg_count,
							0,
							tail
					);
				}
		);
	}
	return offset + BTC_TX_LOCK_TIME_SIZE;
}


size_t BtcTxInBlock::_get_tx_io_count_and_offset(
		size_t offset,
		uint64_t& txin_count,
		uint64_t& txout_count,
		int tail/* = -1*/
) const
{
	offset = get_varint(_buffer, txin_count, offset);
	// TODO : check tail
	offset = _get_tx_inner_array_offset(
			offset,
			txin_count,
			BTC_TX_PREV_OUTPUT_SIZE,
			tail,
			[](size_t offset, size_t count){
				return offset += (count + BTC_TXIN_SEQUENCE_SIZE);
			}
	);
	offset = get_varint(_buffer, txout_count, offset);
	offset = _get_tx_inner_array_offset(
			offset,
			txout_count,
			BTC_TXOUT_VALUE_SIZE,
			tail
	);
	return offset;
}

size_t BtcTxInBlock::_get_tx_inner_array_offset(
		size_t offset,
		size_t count,
		size_t skip_size,
		int tail/* = -1*/,
		std::function<size_t(size_t, size_t)> calc_trail/* =
				[](size_t offset, size_t count){ return offset + count; }*/
) const
{
	uint64_t skip_len = 0;
	for (size_t i = 0 ; i < count ; ++i) {
		offset += skip_size;
		offset = get_varint(_buffer, skip_len, offset);
		offset = calc_trail(offset, skip_len);
		// TODO : check tail
	}
	return offset;
}

} // bitcoin
} // protocols
} // utils
