#pragma once

#include <utils/crypto/sha256.h>
#include <utils/common/byte_array.h>
#include <utils/common/buffer_view.h>

namespace utils {
namespace protocols {

typedef utils::crypto::Sha256 Sha256_t;
typedef utils::common::BufferView ParsedTxContents_t;
typedef std::pair<Sha256_t, ParsedTxContents_t> ParsedTransaction_t;
typedef std::vector<ParsedTransaction_t> ParsedTransactions_t;
typedef utils::common::BufferView TxsMessageContents_t;
typedef std::shared_ptr<TxsMessageContents_t> PTxsMessageContents_t;

class AbstractMessageParser {
public:
    virtual ParsedTransactions_t parse_transactions_message(PTxsMessageContents_t txs_message_contents) const {
        throw std::runtime_error("parse_transactions_message is not implemented in abstract class.");
    }
};

}
}