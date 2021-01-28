#pragma once

#include <utils/protocols/abstract_transaction_validator.h>
#include "utils/common/tx_status_consts.h"
#include "utils/common/tx_validation_status_consts.h"

namespace utils {
namespace protocols {
namespace ontology {

typedef utils::protocols::AbstractTransactionValidator AbstractTransactionValidator_t;

class OntTransactionValidator: public AbstractTransactionValidator_t {
public:
    TxValidationStatus_t transaction_validation(
        const TxContents_t& txs_message_contents,
        const uint64_t min_tx_network_fee,
        const double current_time,
        SenderNonceMap_t& sender_nonce_map,
        const double age,
        const double factor
    ) const;
};

} // ontology
} // protocols
} // utils
