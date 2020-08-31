#include "ont_transaction_validator.h"

namespace utils {
namespace protocols {
namespace ontology {

size_t OntTransactionValidator::transaction_validation(
    const PTxContents_t& txs_message_contents,
    const uint64_t min_tx_network_fee
) const {
    return TX_VALIDATION_STATUS_VALID_TX;
}

} // ontology
} // protocols
} // utils
