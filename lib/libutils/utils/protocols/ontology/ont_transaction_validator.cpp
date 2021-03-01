#include "ont_transaction_validator.h"

namespace utils {
namespace protocols {
namespace ontology {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
TxValidationStatus_t OntTransactionValidator::transaction_validation(
    const PTxContents_t txs_message_contents,
    const uint64_t min_tx_network_fee,
    const double current_time,
    SenderNonceMap_t& sender_nonce_map,
    const double age,
    const double factor

) const {
    return TX_VALIDATION_STATUS_VALID_TX;
}
#pragma GCC diagnostic pop

} // ontology
} // protocols
} // utils
