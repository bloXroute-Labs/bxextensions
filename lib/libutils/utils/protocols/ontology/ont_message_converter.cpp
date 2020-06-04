#include "ont_message_converter.h"
#include "ont_message_helper.h"


namespace utils {
namespace protocols {
namespace ontology {

ParsedTransactions_t OntMessageConverter::tx_to_bx_txs(PTxsMessageContents_t txs_message_contents) const {
    const size_t tx_contents_len = txs_message_contents->size() - ONT_HDR_COMMON_OFF;

    Sha256_t tx_hash = ontology::get_tx_id(
        *txs_message_contents,
        ONT_HDR_COMMON_OFF,
        txs_message_contents->size() - ONT_HDR_COMMON_OFF
    );

    PParsedTxContents_t tx_contents = std::make_shared<ParsedTxContents_t>(
        *txs_message_contents,
        tx_contents_len,
        ONT_HDR_COMMON_OFF
    );

    ParsedTransaction_t parsed_transaction = ParsedTransaction_t(std::move(tx_hash), tx_contents);

    ParsedTransactions_t transactions;
    transactions.push_back(std::move(parsed_transaction));

    return transactions;
}

}
}
}