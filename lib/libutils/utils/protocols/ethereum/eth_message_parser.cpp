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
    uint8_t rlp_type;

    offset = encoding::consume_length_prefix(*msg_buf, tx_bytes_length, rlp_type, offset);
    size_t end = offset + tx_bytes_length;

    uint64_t tx_offset,  tx_end;
    uint64_t tx_length;

    while (offset < end) {
        Sha256_t hash;
        tx_offset = encoding::consume_length_prefix(*msg_buf, tx_length, rlp_type, offset);
        tx_end = tx_offset + tx_length;
        if (rlp_type == RLP_LIST) {
            hash = crypto::keccak_sha3(msg_buf->byte_array(), offset, tx_end - offset);
        } else {
            // this is new type of ETH transaction post EIP 2718 (EIP-2930 for example), the tx is a string that include
            // single byte || rlp list()
            // hash should be calculate on the string content without the rlp encoding
            hash = crypto::keccak_sha3(msg_buf->byte_array(), tx_offset, tx_end - tx_offset);
        }
        ParsedTransaction_t parsed_transaction = ParsedTransaction_t(std::move(hash), tx_end - offset, offset);
        transactions.push_back(std::move(parsed_transaction));

        offset = tx_end;
    }

    return transactions;
}

}
}
}