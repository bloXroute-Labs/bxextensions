#include "utils/protocols/bitcoin/bx_btc_block_message.h"
#include "utils/protocols/bitcoin/btc_consts.h"
#include "utils/common/buffer_helper.h"
#include <utils/common/string_helper.h>

namespace utils {
namespace protocols {
namespace bitcoin {

BxBtcBlockMessage::BxBtcBlockMessage(
        const common::BufferView& buffer,
        uint64_t short_ids_offset
) :
        _bx_block(buffer),
        _short_ids_offset(short_ids_offset),
        _block_message(common::BufferView(
                buffer,
                short_ids_offset - offset_diff,
                offset_diff
        ))
{
}

BxBtcBlockMessage::BxBtcBlockMessage(BxBtcBlockMessage&& rhs) noexcept:
        _bx_block(std::move(rhs._bx_block)),
        _short_ids_offset(rhs._short_ids_offset),
        _block_message(std::move(rhs._block_message))
{
}

BxBtcBlockMessage& BxBtcBlockMessage::operator =(BxBtcBlockMessage&& rhs) noexcept {
    _bx_block = std::move(rhs._bx_block);
    _short_ids_offset = rhs._short_ids_offset;
    _block_message = std::move(rhs._block_message);
    return *this;
}

size_t BxBtcBlockMessage::get_next_tx_offset(
        size_t offset, bool &is_short, size_t& witness_offset
)
{
    is_short = is_short_transaction(offset);
    if (is_short) {
        offset += sizeof(unsigned char);
        witness_offset = 0;
    } else {
        offset = _block_message.get_next_tx_offset(offset - offset_diff, witness_offset);
        offset += offset_diff;
    }
    return offset;
}

bool BxBtcBlockMessage::is_short_transaction(size_t offset) const {
    return _bx_block[offset] == BTC_SHORT_ID_INDICATOR;
}

const BtcBlockMessage &BxBtcBlockMessage::block_message() const {
    return _block_message;
}

size_t BxBtcBlockMessage::get_tx_count(uint64_t &tx_count) {
    return offset_diff + _block_message.get_tx_count(tx_count);
}

void BxBtcBlockMessage::deserialize_short_ids(
        std::vector<unsigned int> &short_ids
)
{
    size_t offset = _short_ids_offset;
    unsigned int short_ids_count = 0;
    offset = utils::common::get_little_endian_value<uint32_t>(
            _bx_block,
            short_ids_count,
            offset
    );
    if (offset + (sizeof(uint32_t) * short_ids_count) > _bx_block.size()) {
        std::string error = utils::common::concatenate(
                "Message is improperly formatted. Expected ",
                short_ids_count,
                " short ids, but not enough space has been allocated."
        );
        throw std::runtime_error(error);
    }
    short_ids.reserve(short_ids_count);
    for (uint32_t idx = 0; idx < short_ids_count; ++idx) {
        uint32_t short_id = 0;
        offset = utils::common::get_little_endian_value<uint32_t>(
                _bx_block,
                short_id,
                offset
        );
        short_ids.push_back(short_id);
    }
}

uint32_t
BxBtcBlockMessage::get_original_block_size(
        const common::BufferView &buffer
)
{
    uint32_t block_size = 0;
    utils::common::get_little_endian_value<uint32_t>(
            buffer,
            block_size,
            offset_diff + BTC_BLOCK_LENGTH_OFFSET
    );
    return block_size + BTC_HDR_COMMON_OFFSET;
}

} // bitcoin
} // protocols
} // utils
