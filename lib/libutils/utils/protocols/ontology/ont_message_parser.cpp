#include "ont_message_parser.h"
#include "ont_message_helper.h"


namespace utils {
namespace protocols {
namespace ontology {

ParsedTransactions_t OntMessageParser::parse_transactions_message(PTxsMessageContents_t msg_buf) const {
    const size_t tx_contents_len = msg_buf->size() - ONT_HDR_COMMON_OFF;

    Sha256_t tx_hash = ontology::get_tx_id(
        *msg_buf,
        ONT_HDR_COMMON_OFF,
        msg_buf->size() - ONT_HDR_COMMON_OFF
    );

    ParsedTransaction_t parsed_transaction = ParsedTransaction_t(
        std::move(tx_hash),
        tx_contents_len,
        ONT_HDR_COMMON_OFF
    );

    ParsedTransactions_t transactions;
    transactions.push_back(std::move(parsed_transaction));

    return std::move(transactions);
}

}
}
}