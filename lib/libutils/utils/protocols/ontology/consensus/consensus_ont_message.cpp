#include <cstdint>
#include "utils/protocols/ontology/consensus/consensus_ont_message.h"
#include "utils/protocols/ontology/consensus/consensus_ont_json_payload.h"
#include "utils/protocols/ontology/ont_consts.h"
#include "utils/protocols/ontology/ont_message_helper.h"
#include "utils/encoding/json_encoder.h"
#include "utils/encoding/base64_encoder.h"
#include "utils/common/string_helper.h"

namespace utils {
namespace protocols {
namespace ontology {
namespace consensus {

ConsensusOntMessage::ConsensusOntMessage(const common::BufferView& buffer):
    _buffer(buffer), _consensus_data_type(0), _header_offset(0), _tx_count(0), _tx_offset(0), _consensus_data_len(0)
{
}

void ConsensusOntMessage::parse() {
    _header_offset = ONT_HDR_COMMON_OFF + 3 * sizeof(uint32_t) + SHA256_BINARY_SIZE + sizeof(uint16_t);
    if (_buffer.size() < _header_offset) {
        throw std::overflow_error(common::concatenate(
                "consensus message buffer size (",
                _buffer.size(),
                ") is smaller than the header size (",
                _header_offset,
                ")!"
        )); // TODO: throw a proper exception here!
    }
    uint64_t ont_payload_len;
    _header_offset = bitcoin::get_varint(_buffer, ont_payload_len, _header_offset);
    size_t owner_and_signature_offset = _header_offset + ont_payload_len;
    if (_buffer.size() < owner_and_signature_offset) {
        throw std::overflow_error(common::concatenate(
                "consensus message buffer size (",
                _buffer.size(),
                ") is smaller than the message size (",
                owner_and_signature_offset,
                ")!"
        )); // TODO: throw a proper exception here!
    }

    std::string consensus_data_str((char *)&_buffer.at(_header_offset), ont_payload_len);
    auto json_payload = encoding::decode_json<ConsensusOntJsonPayload>(consensus_data_str);
    _consensus_data_type = json_payload.type();
    _consensus_data_len = json_payload.len();
    encoding::base64_decode(json_payload.payload(), _block_msg_buffer, 0);
    uint64_t block_msg_len;
    size_t block_msg_offset = bitcoin::get_varint(_block_msg_buffer, block_msg_len, 0);

    size_t block_start_offset = block_msg_offset;
    common::BufferView block_msg_buffer(_block_msg_buffer.byte_array(), _block_msg_buffer.size(), 0);
    block_msg_offset += ONT_TX_VERSION_SIZE;
    _prev_block_hash = crypto::Sha256(block_msg_buffer, block_msg_offset);
    _prev_block_hash.reverse();

    block_msg_offset += (SHA256_BINARY_SIZE * 3 + ONT_BLOCK_TIME_HEIGHT_CONS_DATA_LEN);
    uint64_t consensus_payload_len;
    block_msg_offset = bitcoin::get_varint(_block_msg_buffer, consensus_payload_len, block_msg_offset);
    block_msg_offset += (consensus_payload_len + ONT_BLOCK_NEXT_BOOKKEEPER_LEN);
    _block_hash = std::move(
            crypto::double_sha256(block_msg_buffer, block_start_offset, block_msg_offset - block_start_offset)
    );
    uint64_t bookkeepers_len;
    block_msg_offset = bitcoin::get_varint(_block_msg_buffer, bookkeepers_len, block_msg_offset);
    uint64_t bookkeeper_len;
    for (uint64_t i = 0 ; i < bookkeepers_len ; ++i) {
        block_msg_offset = bitcoin::get_varint(_block_msg_buffer, bookkeeper_len, block_msg_offset);
        block_msg_offset += ONT_BOOKKEEPER_LEN;
    }
    uint64_t sig_data_len;
    block_msg_offset = bitcoin::get_varint(_block_msg_buffer, sig_data_len, block_msg_offset);
    uint64_t sig_length;
    for (uint64_t i = 0 ; i < sig_data_len ; ++i) {
        block_msg_offset = bitcoin::get_varint(_block_msg_buffer, sig_length, block_msg_offset);
        block_msg_offset += sig_length;
    }
    block_msg_offset = common::get_little_endian_value<uint32_t >(_block_msg_buffer, _tx_count, block_msg_offset);
    _tx_offset = block_msg_offset;
    _owner_and_signature = common::BufferView(
            _buffer, _buffer.size() - owner_and_signature_offset, owner_and_signature_offset
    );
    size_t payload_tail_offset = block_start_offset + block_msg_len;
    _payload_tail = common::BufferView(
            _block_msg_buffer.byte_array(), _consensus_data_len - payload_tail_offset, payload_tail_offset
    );

}

crypto::Sha256 ConsensusOntMessage::block_hash() const {
    return _block_hash;
}

crypto::Sha256 ConsensusOntMessage::prev_block_hash() const {
    return _prev_block_hash;
}

size_t ConsensusOntMessage::get_next_tx_offset(size_t offset) {
    uint8_t tx_type;
    offset = get_tx_type(_block_msg_buffer, offset, tx_type);
    uint64_t sig_length;
    offset = bitcoin::get_varint(_block_msg_buffer, sig_length, offset);
    for (uint64_t i = 0 ; i < sig_length ; ++i) {
        uint64_t invoke_length, verify_length;
        offset = bitcoin::get_varint(_block_msg_buffer, invoke_length, offset);
        offset += invoke_length;
        offset = bitcoin::get_varint(_block_msg_buffer, verify_length, offset);
        offset += verify_length;
    }
    return offset;
}

size_t ConsensusOntMessage::get_tx_count(uint32_t& tx_count) {
    tx_count = _tx_count;
    return _tx_offset;
}

const common::BufferView& ConsensusOntMessage::owner_and_signature() const {
    return _owner_and_signature;
}

const common::BufferView& ConsensusOntMessage::payload_tail() const {
    return _payload_tail;
}

common::BufferView ConsensusOntMessage::block_msg_buffer() const {
    return std::move(common::BufferView(_block_msg_buffer.byte_array(), _block_msg_buffer.size()));
}

size_t ConsensusOntMessage::get_header_offset() const {
    return _header_offset;
}

uint8_t ConsensusOntMessage::consensus_data_type() const {
    return _consensus_data_type;
}

uint32_t ConsensusOntMessage::consensus_data_len() const {
    return _consensus_data_len;
}

} // consensus
} // ontology
} // protocols
} // utils