#include "utils/protocols/bitcoin/btc_message_helper.h"
#include "utils/protocols/ontology/ont_message_helper.h"
#include "utils/common/buffer_helper.h"
#include "utils/protocols/ontology/consensus/bx_ont_consensus_message.h"


namespace utils {
namespace protocols {
namespace ontology {
namespace consensus {

BxOntConsensusMessage::BxOntConsensusMessage(const common::BufferView& buffer):
    _buffer(buffer), _consensus_data_len(0), _consensus_data_type(0), _tx_offset(0), _tx_count(0), _short_ids_offset(0)
{
}

void BxOntConsensusMessage::parse() {
    size_t offset = common::get_little_endian_value<uint64_t >(_buffer, _short_ids_offset, 0);
    offset  += sizeof(uint8_t);
    _block_hash = std::move(crypto::Sha256(_buffer, offset));
    offset += SHA256_BINARY_SIZE;
    offset = common::get_little_endian_value<uint32_t >(_buffer, _tx_count, offset);
    uint32_t payload_tail_len;
    offset = common::get_little_endian_value<uint32_t >(_buffer, payload_tail_len, offset);
    _payload_tail = common::BufferView(_buffer, payload_tail_len, offset);
    offset += payload_tail_len;
    uint32_t owner_and_signature_len;
    offset = common::get_little_endian_value<uint32_t >(_buffer, owner_and_signature_len, offset);
    _owner_and_signature = common::BufferView(_buffer, owner_and_signature_len, offset);
    offset += owner_and_signature_len;
    uint32_t consensus_payload_header_len;
    offset = common::get_little_endian_value<uint32_t>(_buffer, consensus_payload_header_len, offset);
    _consensus_payload_header = common::BufferView(_buffer, consensus_payload_header_len, offset);
    offset += consensus_payload_header_len;
    offset = common::get_little_endian_value<uint8_t>(_buffer, _consensus_data_type, offset);
    offset = common::get_little_endian_value<uint32_t>(_buffer, _consensus_data_len, offset);
    uint32_t header_len;
    offset = common::get_little_endian_value<uint32_t >(_buffer, header_len, offset);
    _header = common::BufferView(_buffer,  header_len, offset);
    offset += header_len;
    _tx_offset = offset;
}

crypto::Sha256 BxOntConsensusMessage::block_hash() const {
    return _block_hash;
}

bool BxOntConsensusMessage::is_short_transaction(size_t offset) const {
    return _buffer[offset] == BTC_SHORT_ID_INDICATOR;
}

size_t BxOntConsensusMessage::get_next_tx_offset(size_t offset, bool& is_short) {
    is_short = is_short_transaction(offset);
    if (is_short) {
        offset += sizeof(unsigned char);
    } else {
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
    }
    return offset;
}

size_t BxOntConsensusMessage::get_tx_count(uint32_t& tx_count) {
    tx_count = _tx_count;
    return _tx_offset;
}

uint32_t BxOntConsensusMessage::get_original_block_size() const {
    return _header.size() + _consensus_data_len + _owner_and_signature.size();
}

const common::BufferView& BxOntConsensusMessage::header() const {
    return _header;
}

const common::BufferView& BxOntConsensusMessage::owner_and_signature() const {
    return _owner_and_signature;
}

const common::BufferView& BxOntConsensusMessage::payload_tail() const {
    return _payload_tail;
}

const common::BufferView& BxOntConsensusMessage::consensus_payload_header() const {
    return _consensus_payload_header;
}

uint8_t BxOntConsensusMessage::consensus_data_type() const {
    return _consensus_data_type;
}

uint32_t BxOntConsensusMessage::consensus_data_len() const {
    return _consensus_data_len;
}

void BxOntConsensusMessage::deserialize_short_ids(
        std::vector<unsigned int> &short_ids
)
{
    size_t offset = _short_ids_offset;
    unsigned int short_ids_count = 0;
    offset = utils::common::get_little_endian_value<uint32_t>(
            _buffer,
            short_ids_count,
            offset
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
                _buffer,
                short_id,
                offset
        );
        short_ids.push_back(short_id);
    }
}

} // consensus
} // ontology
} // protocols
} // utils
