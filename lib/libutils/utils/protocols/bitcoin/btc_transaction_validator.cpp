#include "btc_transaction_validator.h"

namespace utils {
namespace protocols {
namespace bitcoin {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
TxValidationStatus_t BtcTransactionValidator::transaction_validation(
    const TxContents_t& txs_message_contents,
    const uint64_t min_tx_network_fee,
    const double current_time,
    SenderNonceMap_t& sender_nonce_map,
    const double age,
    const double factor
) const {
    return TX_VALIDATION_STATUS_VALID_TX;
}
#pragma GCC diagnostic pop

} // bitcoin
} // protocols
} // utils
