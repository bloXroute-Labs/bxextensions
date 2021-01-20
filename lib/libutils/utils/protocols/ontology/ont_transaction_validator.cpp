#include "ont_transaction_validator.h"

namespace utils {
namespace protocols {
namespace ontology {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
size_t OntTransactionValidator::transaction_validation(
    const PTxContents_t& txs_message_contents,
    const uint64_t min_tx_network_fee,
        const double current_time,
    SenderNonceToTime_t& sender_nonce_to_time
) const {
    return TX_VALIDATION_STATUS_VALID_TX;
}
#pragma GCC diagnostic pop

} // ontology
} // protocols
} // utils
