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
    size_t& witness_offset
) const
{
    offset += BTC_TX_VERSION_SIZE;
	auto segwit = get_segwit(_buffer, offset);
	uint64_t tx_in_count = 0, tx_out_count = 0;
    offset = _get_tx_io_count_and_offset(
			segwit, tx_in_count, tx_out_count, witness_offset
	);
	return offset + BTC_TX_LOCK_TIME_SIZE;
}


size_t BtcTxInBlock::_get_tx_io_count_and_offset(
        const std::pair<bool, size_t>& segwit,
		uint64_t& tx_in_count,
		uint64_t& tx_out_count,
        size_t& witness_offset
) const
{
    size_t offset = segwit.second;
    bool is_segwit = segwit.first;
    uint64_t script_size;
    offset = get_varint(_buffer, tx_in_count, offset);
    for (uint64_t idx = 0 ; idx < tx_in_count ; ++idx) {
        offset += BTC_TX_PREV_OUTPUT_SIZE;
        offset = get_varint(_buffer, script_size, offset) + BTC_TXIN_SEQUENCE_SIZE;
        offset += script_size;
    }
    offset = get_varint(_buffer, tx_out_count, offset);
    for (uint64_t idx = 0 ; idx < tx_out_count ; ++idx) {
        offset += BTC_TXOUT_VALUE_SIZE;
        offset = get_varint(_buffer, script_size, offset);
        offset += script_size;
    }
    witness_offset = offset;
    if (is_segwit) {
        uint64_t witness_count, witness_size;
        for (uint64_t idx = 0 ; idx < tx_in_count ; ++idx) {
            offset = get_varint(_buffer, witness_count, offset);
            for (uint64_t i = 0 ; i < witness_count ; ++i) {
                offset = get_varint(_buffer, witness_size, offset);
                offset += witness_size;
            }
        }
    }
	return offset;
}

} // bitcoin
} // protocols
} // utils
