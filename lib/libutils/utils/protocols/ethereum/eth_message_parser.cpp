#include <utils/encoding/rlp_encoder.h>
#include <utils/crypto/keccak.h>

#include "eth_message_parser.h"

namespace utils {
namespace protocols {
namespace ethereum {

ParsedTransactions_t EthMessageParser::parse_transactions_message(PTxsMessageContents_t msg_buf) const {
    ParsedTransactions_t transactions;

    uint64_t offset = 0;
    uint64_t tx_bytes_length;

    offset = encoding::get_length_prefix(*msg_buf, tx_bytes_length, offset);
    size_t end = offset + tx_bytes_length;

    uint64_t tx_offset, tx_length, tx_end;

    while (offset < end) {
        tx_offset = encoding::get_length_prefix(*msg_buf, tx_length, offset);
        tx_end = tx_offset + tx_length;

        Sha256_t hash = crypto::keccak_sha3(msg_buf->byte_array(), offset, tx_end - offset);

        ParsedTxContents_t tx_contents = ParsedTxContents_t(
            *msg_buf,
            tx_end - offset,
            offset
        );

        offset = tx_end;

        ParsedTransaction_t parsed_transaction = ParsedTransaction_t(std::move(hash), std::move(tx_contents));
        transactions.push_back(std::move(parsed_transaction));
    }

    return std::move(transactions);
}

}
}
}