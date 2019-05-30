#include "utils/protocols/bitcoin/bx_btc_block_message.h"
#include "utils/protocols/bitcoin/btc_consts.h"
#include "utils/common/buffer_helper.h"

namespace utils {
namespace protocols {

BxBtcBlockMessage::BxBtcBlockMessage(
		const common::BufferView& buffer,
		uint64_t short_ids_offset
):
		_block_message(common::BufferView(
				buffer,
				buffer.size() - short_ids_offset - offset_diff,
				offset_diff
		)),
		_bx_block(buffer),
		_short_ids_offset(short_ids_offset)
{
}

size_t BxBtcBlockMessage::get_next_tx_offset(
		size_t offset, bool& is_short, int tail/* = -1*/
)
{
	is_short = is_short_transaction(offset);
	if (is_short) {
		offset += sizeof(unsigned char);
	} else {
		offset = _block_message.get_next_tx_offset(offset - offset_diff, tail);
		offset += offset_diff;
	}
	return offset;
}

bool BxBtcBlockMessage::is_short_transaction(size_t offset) const {
	return _bx_block[offset] == BTC_SHORT_ID_INDICATOR;
}

const BTCBlockMessage& BxBtcBlockMessage::block_message() const {
	return _block_message;
}

size_t BxBtcBlockMessage::get_tx_count(uint64_t& tx_count) {
	return offset_diff + _block_message.get_tx_count(tx_count);
}

void BxBtcBlockMessage::deserialize_short_ids(
		std::vector<unsigned int>& short_ids
)
{
	size_t offset = _short_ids_offset;
	unsigned int short_ids_count = 0;
	offset = utils::common::get_little_endian_value<uint32_t>(
			_bx_block,
			short_ids_count,
			offset
	);
	short_ids.reserve(short_ids_count);
	for (uint32_t idx = 0 ; idx < short_ids_count ; ++idx) {
		uint32_t short_id = 0;
		offset = utils::common::get_little_endian_value<uint32_t>(
				_bx_block,
				short_id,
				offset
		);
		short_ids.push_back(short_id);
	}
}

} // protocols
} // utils
