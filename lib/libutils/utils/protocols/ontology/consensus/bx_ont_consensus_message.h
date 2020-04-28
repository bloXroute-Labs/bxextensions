#include <iostream>
#include "utils/protocols/ontology/consensus/ont_consensus_message.h"

#ifndef UTILS_PROTOCOLS_ONTOLOGY_CONSENSUS_BX_ONT_CONSENSUS_MESSAGE_H_
#define UTILS_PROTOCOLS_ONTOLOGY_CONSENSUS_BX_ONT_CONSENSUS_MESSAGE_H_

namespace utils {
namespace protocols {
namespace ontology {
namespace consensus {

class BxOntConsensusMessage {
public:
    explicit BxOntConsensusMessage(const common::BufferView& buffer);

    void parse();

    crypto::Sha256 block_hash() const;

    bool is_short_transaction(size_t offset) const;

    size_t get_next_tx_offset(size_t offset, bool& is_short);
    size_t get_tx_count(uint32_t& tx_count);

    uint32_t consensus_data_len() const;
    uint32_t get_original_block_size() const;

    const common::BufferView& header() const;
    const common::BufferView& owner_and_signature() const;
    const common::BufferView& payload_tail() const;
    const common::BufferView& consensus_payload_header() const;

    uint8_t consensus_data_type() const;

    void deserialize_short_ids(std::vector<unsigned int> &short_ids);

private:
    common::BufferView _buffer, _owner_and_signature, _payload_tail, _header, _consensus_payload_header;
    uint8_t _consensus_data_type;
    crypto::Sha256 _block_hash;
    uint32_t _tx_count, _consensus_data_len;
    size_t _tx_offset;
    uint64_t _short_ids_offset;

};

} // consensus
} // ontology
} // protocols
} // utils

#endif //UTILS_PROTOCOLS_ONTOLOGY_CONSENSUS_BX_ONT_CONSENSUS_MESSAGE_H_
