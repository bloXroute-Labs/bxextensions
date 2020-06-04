#pragma once

#include <utils/crypto/sha256.h>
#include <utils/common/byte_array.h>
#include <utils/common/buffer_view.h>

namespace utils {
namespace protocols {

typedef utils::crypto::Sha256 Sha256_t;
typedef utils::common::BufferView ParsedTxContents_t;
typedef std::shared_ptr<ParsedTxContents_t> PParsedTxContents_t;
typedef std::pair<Sha256_t, PParsedTxContents_t> ParsedTransaction_t;
typedef std::vector<ParsedTransaction_t> ParsedTransactions_t;
typedef utils::common::BufferView TxsMessageContents_t;
typedef std::shared_ptr<TxsMessageContents_t> PTxsMessageContents_t;

class AbstractMessageConverter {
public:
    virtual ParsedTransactions_t tx_to_bx_txs(PTxsMessageContents_t txs_message_contents) const {
        throw std::runtime_error("tx_to_bx_txs is not implemented in abstract class.");
    }
};

}
}