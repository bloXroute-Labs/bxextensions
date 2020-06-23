#include "utils/protocols/ethereum/bx_eth_block_message.h"
#include "utils/protocols/ethereum/eth_consts.h"
#include <utils/common/buffer_helper.h>
#include <utils/common/string_helper.h>
#include <utils/encoding/rlp_encoder.h>
#include <utils/crypto/keccak.h>

namespace utils {
namespace protocols {
namespace ethereum {

BxEthBlockMessage::BxEthBlockMessage(const common::BufferView& buffer) :
        _buffer(buffer), _short_ids_offset(0), _txn_offset(0), _txn_end_offset(0)
{
}

BxEthBlockMessage::BxEthBlockMessage(BxEthBlockMessage&& rhs) noexcept:
        _block_header(rhs._block_header),
        _block_trailer(rhs._block_trailer),
        _buffer(std::move(rhs._buffer)),
        _short_ids_offset(rhs._short_ids_offset),
        _block_hash(std::move(rhs._block_hash)),
        _txn_offset(rhs._txn_offset),
        _txn_end_offset(rhs._txn_end_offset)
{
}

BxEthBlockMessage& BxEthBlockMessage::operator =(BxEthBlockMessage&& rhs) noexcept {
    _buffer = std::move(rhs._buffer);
    _short_ids_offset = rhs._short_ids_offset;
    _block_header = std::move(rhs._block_header);
    _block_trailer = std::move(rhs._block_trailer);
    _block_hash = std::move(rhs._block_hash);
    _txn_offset = rhs._txn_offset;
    _txn_end_offset = rhs._txn_end_offset;
    return *this;
}

size_t BxEthBlockMessage::get_next_tx_offset(
    size_t offset, size_t& from, bool& is_short
)
{
    uint64_t len;
    from = encoding::consume_length_prefix(_buffer, len, offset);
    is_short = _buffer[from] == ETH_SHORT_ID_INDICATOR;
    return from + len;
}

crypto::Sha256 BxEthBlockMessage::block_hash() const {
    return _block_hash;
}

const common::BufferView &BxEthBlockMessage::block_header() const {
    return _block_header;
}

const common::BufferView &BxEthBlockMessage::block_trailer() const {
    return _block_trailer;
}

size_t BxEthBlockMessage::txn_end_offset() const {
    return _txn_end_offset;
}

size_t BxEthBlockMessage::txn_offset() const {
    return _txn_offset;
}

void BxEthBlockMessage::parse()
{
    uint64_t block_item_len, block_header_len, block_txs_len;
    size_t short_ids_offset = common::get_little_endian_value<uint64_t >(_buffer, _short_ids_offset, 0);

    size_t block_item_offset = encoding::consume_length_prefix(_buffer, block_item_len, short_ids_offset);

    size_t block_header_offset = encoding::consume_length_prefix(_buffer, block_header_len, block_item_offset);
    _block_header = common::BufferView(
        _buffer, block_header_offset + block_header_len - block_item_offset, block_item_offset
    );
    _block_hash = crypto::Sha256(
        utils::crypto::keccak_sha3((uint8_t*)_block_header.char_array(), 0, _block_header.size())
    );

    _txn_offset = encoding::consume_length_prefix(_buffer, block_txs_len, block_header_offset + block_header_len);

    _txn_end_offset = _txn_offset + block_txs_len;

    _block_trailer = common::BufferView(_buffer, _short_ids_offset - _txn_end_offset, _txn_end_offset);
}

void BxEthBlockMessage::deserialize_short_ids(
    std::vector<unsigned int> &short_ids
)
{
    size_t offset = _short_ids_offset;
    unsigned int short_ids_count = 0;
    offset = utils::common::get_little_endian_value<uint32_t>(
        _buffer, short_ids_count, offset
    );
    if (offset + (sizeof(uint32_t) * short_ids_count) > _buffer.size()) {
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
            _buffer, short_id, offset
        );
        short_ids.push_back(short_id);
    }
}
} // ethereum
} // protocols
} // utils
