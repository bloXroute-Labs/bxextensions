#include "utils/protocols/ontology/ont_block_message.h"
#include "utils/protocols/ontology/ont_message_helper.h"

namespace utils {
namespace protocols {
namespace ontology {

OntBlockMessage::OntBlockMessage(const common::BufferView& buffer): _buffer(buffer) {
    _header_offset = ONT_HDR_COMMON_OFF + sizeof(uint32_t) + SHA256_BINARY_SIZE * 3 +
            ONT_BLOCK_TIME_HEIGHT_CONS_DATA_LEN;
    uint64_t consensus_payload_length;
    _header_offset = bitcoin::get_varint(_buffer, consensus_payload_length, _header_offset);
    _header_offset += (consensus_payload_length + ONT_BLOCK_NEXT_BOOKKEEPER_LEN);
    uint64_t bookkeepers_length;
    _tx_count_offset = bitcoin::get_varint(_buffer, bookkeepers_length, _header_offset);
    uint64_t tmp;
    for (uint64_t i = 0 ; i < bookkeepers_length ; ++i) {
        _tx_count_offset = bitcoin::get_varint(_buffer, tmp, _tx_count_offset);
        _tx_count_offset += ONT_BOOKKEEPER_LEN;
    }
    uint64_t sig_data_length, sig_length;
    _tx_count_offset = bitcoin::get_varint(_buffer, sig_data_length, _tx_count_offset);
    for (uint64_t i = 0 ; i < sig_data_length ; ++i) {
        _tx_count_offset = bitcoin::get_varint(_buffer, sig_length, _tx_count_offset);
        _tx_count_offset += sig_length;
    }
}

OntBlockMessage::OntBlockMessage(OntBlockMessage&& rhs) noexcept {
    _buffer = std::move(rhs._buffer);
    _header_offset = rhs._header_offset;
    _tx_count_offset = rhs._tx_count_offset;
}

OntBlockMessage& OntBlockMessage::operator =(OntBlockMessage&& rhs) noexcept {
    _buffer = std::move(rhs._buffer);
    _header_offset = rhs._header_offset;
    _tx_count_offset = rhs._tx_count_offset;
    return *this;
}

size_t OntBlockMessage::get_next_tx_offset(size_t offset) {
    uint8_t tx_type;
    offset = get_tx_type(_buffer, offset, tx_type);
    uint64_t sig_length;
    offset = bitcoin::get_varint(_buffer, sig_length, offset);
    for (uint64_t i = 0 ; i < sig_length ; ++i) {
        uint64_t invoke_length, verify_length;
        offset = bitcoin::get_varint(_buffer, invoke_length, offset);
        offset += invoke_length;
        offset = bitcoin::get_varint(_buffer, verify_length, offset);
        offset += verify_length;
    }
    return offset;
}

size_t OntBlockMessage::get_txn_count(uint32_t& tx_count) {
    return common::get_little_endian_value<uint32_t>(_buffer, tx_count, _tx_count_offset);
}

crypto::Sha256 OntBlockMessage::block_hash() const {
    return generate_block_hash(_buffer, _header_offset);
}

crypto::Sha256 OntBlockMessage::prev_block_hash() const {
    return get_prev_block_hash(_buffer);
}

common::BufferView OntBlockMessage::merkle_root() const {
    return get_merkle_root(_buffer);
}

} // ontology
} // protocols
} // utils