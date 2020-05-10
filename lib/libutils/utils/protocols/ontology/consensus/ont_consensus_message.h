#include <iostream>

#include "utils/common/buffer_view.h"
#include "utils/common/byte_array.h"
#include "utils/crypto/sha256.h"

#ifndef UTILS_PROTOCOLS_ONTOLOGY_ONT_CONSENSUS_CONSENSUS_MESSAGE_H_
#define UTILS_PROTOCOLS_ONTOLOGY_ONT_CONSENSUS_CONSENSUS_MESSAGE_H_

namespace utils {
namespace protocols {
namespace ontology {
namespace consensus {

class OntConsensusMessage {
public:
    explicit OntConsensusMessage(const common::BufferView& buffer);

    void parse();

    crypto::Sha256 block_hash() const;
    crypto::Sha256 prev_block_hash() const;

    size_t get_next_tx_offset(size_t offset);
    size_t get_txn_count(uint32_t& txn_count);
    size_t get_header_offset() const;

    const common::BufferView& owner_and_signature() const;
    const common::BufferView& payload_tail() const;

    common::BufferView block_msg_buffer() const;

    uint8_t consensus_data_type() const;

    uint32_t consensus_data_len() const;

private:
    common::BufferView _buffer, _owner_and_signature, _payload_tail;
    uint8_t _consensus_data_type;
    common::ByteArray _block_msg_buffer;
    size_t _header_offset;
    crypto::Sha256 _prev_block_hash, _block_hash;
    uint32_t _txn_count, _consensus_data_len;
    size_t _tx_offset;

};

} // consensus
} // ontology
} // protocols
} // utils

#endif //UTILS_PROTOCOLS_ONTOLOGY_ONT_CONSENSUS_CONSENSUS_MESSAGE_H_
