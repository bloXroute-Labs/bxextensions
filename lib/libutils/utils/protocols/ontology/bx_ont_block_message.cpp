#include "utils/protocols/ontology/bx_ont_block_message.h"
#include "utils/protocols/bitcoin/btc_consts.h"
#include "utils/protocols/ontology/ont_consts.h"
#include "utils/protocols/ontology/ont_message_helper.h"
#include "utils/common/buffer_helper.h"
#include "utils/common/string_helper.h"

namespace utils {
namespace protocols {
namespace ontology {

BxOntBlockMessage::BxOntBlockMessage(
        const common::BufferView& buffer,
        uint64_t short_ids_offset
) :
        _block_message(common::BufferView(
                buffer,
                short_ids_offset - offset_diff,
                offset_diff
        )),
        _bx_block(buffer),
        _short_ids_offset(short_ids_offset)
{
}

BxOntBlockMessage::BxOntBlockMessage(BxOntBlockMessage&& rhs) noexcept:
        _block_message(std::move(rhs._block_message)),
        _bx_block(std::move(rhs._bx_block)),
        _short_ids_offset(rhs._short_ids_offset)
{
}

BxOntBlockMessage& BxOntBlockMessage::operator =(BxOntBlockMessage&& rhs) noexcept {
    _block_message = std::move(rhs._block_message);
    _bx_block = std::move(rhs._bx_block);
    _short_ids_offset = rhs._short_ids_offset;
    return *this;
}

size_t BxOntBlockMessage::get_next_tx_offset(size_t offset, bool &is_short) {
    is_short = is_short_transaction(offset);
    if (is_short) {
        offset += sizeof(unsigned char);
    } else {
        offset = _block_message.get_next_tx_offset(offset - offset_diff);
        offset += offset_diff;
    }
    return offset;
}

bool BxOntBlockMessage::is_short_transaction(size_t offset) const {
    return _bx_block[offset] == BTC_SHORT_ID_INDICATOR;
}

const OntBlockMessage &BxOntBlockMessage::block_message() const {
    return _block_message;
}

size_t BxOntBlockMessage::get_tx_count(uint32_t &tx_count) {
    return offset_diff + _block_message.get_tx_count(tx_count);
}

void BxOntBlockMessage::deserialize_short_ids(
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

uint32_t BxOntBlockMessage::get_original_block_size(const common::BufferView &buffer) {
    return get_block_size(buffer, offset_diff);
}

common::BufferView BxOntBlockMessage::merkle_root() const {
    return std::move(common::BufferView(_bx_block, SHA256_BINARY_SIZE, ONT_BX_MERKLE_ROOT_OFFSET));
}

} // ontology
} // protocols
} // utils
