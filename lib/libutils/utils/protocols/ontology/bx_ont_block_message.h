#include <iostream>

#include "utils/protocols/ontology/ont_block_message.h"

#ifndef UTILS_PROTOCOLS_ONTOLOGY_BX_ONT_BLOCK_MESSAGE_H_
#define UTILS_PROTOCOLS_ONTOLOGY_BX_ONT_BLOCK_MESSAGE_H_

namespace utils {
namespace protocols {
namespace ontology {

class BxOntBlockMessage {
public:
    BxOntBlockMessage(
            const common::BufferView& buffer, uint64_t short_ids_offset
    );
    BxOntBlockMessage(BxOntBlockMessage&&) noexcept;
    BxOntBlockMessage& operator =(BxOntBlockMessage&&) noexcept;

    size_t get_next_tx_offset(size_t offset, bool& is_short);
    bool is_short_transaction(size_t offset) const;

    const OntBlockMessage& block_message() const;
    size_t get_tx_count(uint32_t& tx_count);
    void deserialize_short_ids(std::vector<unsigned int>& short_ids);

    static uint32_t get_original_block_size(const common::BufferView& buffer);

    common::BufferView merkle_root() const;

    static constexpr size_t offset_diff = sizeof(uint64_t) + SHA256_BINARY_SIZE;
private:
    common::BufferView _bx_block;
    uint64_t _short_ids_offset;
    OntBlockMessage _block_message;
};

} // ontology
} // protocols
} // utils

#endif //UTILS_PROTOCOLS_ONTOLOGY_BX_ONT_BLOCK_MESSAGE_H_
