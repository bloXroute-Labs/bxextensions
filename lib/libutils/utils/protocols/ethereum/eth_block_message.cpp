#include <array>
#include "utils/protocols/ethereum/eth_consts.h"
#include "utils/protocols/ethereum/eth_block_message.h"
#include <utils/encoding/rlp_encoder.h>
#include <utils/crypto/keccak.h>
#include <utils/common/buffer_view.h>

namespace utils {
namespace protocols {
namespace ethereum {

EthBlockMessage::EthBlockMessage(const common::BufferView& buffer): _buffer(buffer) {
}

EthBlockMessage::EthBlockMessage(EthBlockMessage&& rhs) noexcept:
    _buffer(std::move(rhs._buffer)),
    _block_header(std::move(rhs._block_header)),
    _block_trailer(std::move(rhs._block_trailer)),
    _block_hash(std::move(rhs._block_hash)),
    _prev_block_hash(std::move(rhs._prev_block_hash)),
    _txn_offset(rhs._txn_offset),
    _txn_end_offset(rhs._txn_end_offset)
{
}

EthBlockMessage& EthBlockMessage::operator =(EthBlockMessage&& rhs) noexcept {
    _buffer = std::move(rhs._buffer);
    _block_header = std::move(rhs._block_header);
    _block_trailer = std::move(rhs._block_trailer);
    _block_hash = std::move(rhs._block_hash);
    _prev_block_hash = std::move(rhs._prev_block_hash);
    _txn_offset = rhs._txn_offset;
    _txn_end_offset = rhs._txn_end_offset;
    return *this;
}

size_t EthBlockMessage::get_next_tx_offset(size_t offset, uint8_t& rlp_type) {
    uint64_t len;
    size_t from = encoding::consume_length_prefix(_buffer, len, rlp_type, offset);
    //    if (_buffer.at(offset) < START_RANGE_ETH_LEGACY_TX) {
//        offset++;
//    }

    return from + len;
}

crypto::Sha256 EthBlockMessage::block_hash() const {
    return _block_hash;
}

crypto::Sha256 EthBlockMessage::prev_block_hash() const {
    return _prev_block_hash;
}

void EthBlockMessage::parse() {
    uint64_t block_msg_item_len, block_header_item_len, prev_block_item_len, txn_item_len;
    uint8_t rlp_type;

    size_t block_msg_item_offset = encoding::consume_length_prefix(
        _buffer, block_msg_item_len, rlp_type,0
    );

    size_t block_header_item_offset = encoding::consume_length_prefix(
        _buffer, block_header_item_len, rlp_type, block_msg_item_offset
    );

    _block_header = common::BufferView(
        _buffer, block_header_item_len + block_header_item_offset - block_msg_item_offset, block_msg_item_offset
    );

    _block_hash = crypto::Sha256(
        utils::crypto::keccak_sha3(
            (uint8_t*)_block_header.char_array(), 0, _block_header.size()
        )
    );

    size_t prev_block_item_offset = encoding::consume_length_prefix(
        _buffer, prev_block_item_len, rlp_type, block_header_item_offset
    );

    _prev_block_hash = crypto::Sha256(_buffer, prev_block_item_offset);

    _txn_offset = encoding::consume_length_prefix(
        _buffer, txn_item_len, rlp_type, block_header_item_offset + block_header_item_len
    );

    _txn_end_offset = _txn_offset + txn_item_len;
    _block_trailer = common::BufferView(_buffer, _buffer.size() - _txn_end_offset, _txn_end_offset);
}

size_t EthBlockMessage::txn_end_offset() const {
    return _txn_end_offset;
}

size_t EthBlockMessage::txn_offset() const {
    return _txn_offset;
}

const common::BufferView& EthBlockMessage::block_header() const {
    return _block_header;
}

const common::BufferView& EthBlockMessage::block_trailer() const {
    return _block_trailer;
}

} // ethereum
} // protocols
} // utils
