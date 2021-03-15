#pragma once

#include <utils/protocols/abstract_transaction_validator.h>
#include "utils/common/tx_status_consts.h"
#include "utils/common/tx_validation_status_consts.h"
#include "eth_tx_message.h"

namespace utils {
namespace protocols {
namespace ethereum {

typedef utils::protocols::AbstractTransactionValidator AbstractTransactionValidator_t;
typedef utils::protocols::ethereum::EthTxMessage EthTxMessage_t;

class EthTransactionValidator: public AbstractTransactionValidator_t {
public:
    bool _parse_transaction(const PTxContents_t& txs_message_contents, EthTxMessage_t& tx_msg) const;
    bool _verify_transaction_signature(EthTxMessage tx_msg, Sha256_t& sender_nonce_key) const;
    TxValidationStatus_t transaction_validation(
        const PTxContents_t txs_message_contents,
        const uint64_t min_tx_network_fee,
        const double current_time,
        SenderNonceMap_t& sender_nonce_map,
        const double age,
        const double factor
    ) const;
};

} // ethereum
} // protocols
} // utils