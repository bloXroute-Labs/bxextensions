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
    size_t transaction_validation(
        const PTxContents_t& txs_message_contents,
        const uint64_t min_tx_network_fee,
        const double current_time,
        SenderNonceToTime_t& sender_nonce_to_time
    ) const;
};

} // ontology
} // protocols
} // utils
