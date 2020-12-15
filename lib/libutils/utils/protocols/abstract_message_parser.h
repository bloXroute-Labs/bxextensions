#pragma once

#include <utils/crypto/sha256.h>
#include <utils/common/byte_array.h>
#include <utils/common/buffer_view.h>

namespace utils {
namespace protocols {

typedef utils::crypto::Sha256 Sha256_t;

struct ParsedTransaction {
    Sha256_t transaction_hash;
    size_t length;
    size_t offset;

    ParsedTransaction(
        Sha256_t tx_hash,
        size_t tx_length,
        size_t tx_offset
    ) :
        transaction_hash(tx_hash),
        length(tx_length),
        offset(tx_offset) {}
};

typedef ParsedTransaction ParsedTransaction_t;
typedef std::vector<ParsedTransaction_t> ParsedTransactions_t;
typedef utils::common::BufferView TxsMessageContents_t;
typedef std::shared_ptr<TxsMessageContents_t> PTxsMessageContents_t;

class AbstractMessageParser {
public:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
    virtual ParsedTransactions_t parse_transactions_message(PTxsMessageContents_t msg_buf) const {
        throw std::runtime_error("parse_transactions_message is not implemented in abstract class.");
    }
#pragma GCC diagnostic pop
};

}
}