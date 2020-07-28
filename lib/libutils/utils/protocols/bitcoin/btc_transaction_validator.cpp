#include "btc_transaction_validator.h"

namespace utils {
namespace protocols {
namespace bitcoin {

size_t BtcTransactionValidator::transaction_validation(const PTxContents_t& txs_message_contents) const {
    return TX_VALIDATION_STATUS_VALID_TX;
}

} // bitcoin
} // protocols
} // utils
