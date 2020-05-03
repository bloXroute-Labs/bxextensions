#include <iostream>
#include <vector>
#include <cstdint>
#include <functional>

#include "utils/crypto/sha256.h"

#ifndef UTILS_PROTOCOLS_ONTOLOGY_ONT_BLOCK_MESSAGE_H_
#define UTILS_PROTOCOLS_ONTOLOGY_ONT_BLOCK_MESSAGE_H_

namespace utils {
namespace protocols {
namespace ontology {

class OntBlockMessage {
public:
    explicit OntBlockMessage(const common::BufferView& buffer);
    OntBlockMessage(OntBlockMessage&&) noexcept;

    OntBlockMessage& operator =(OntBlockMessage&&) noexcept;

    size_t get_next_tx_offset(size_t offset);
    size_t get_txn_count(uint32_t& tx_count);

    crypto::Sha256 block_hash() const;
    crypto::Sha256 prev_block_hash() const;
    common::BufferView merkle_root() const;

private:
    common::BufferView _buffer;
    size_t _header_offset;
    size_t _tx_count_offset;
};

} // ontology
} // protocols
} // utils

#endif /* UTILS_PROTOCOLS_ONTOLOGY_ONT_BLOCK_MESSAGE_H_ */