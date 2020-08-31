#include "eth_transaction_validator.h"
#include <utils/crypto/signature.h>

namespace utils {
namespace protocols {
namespace ethereum {

bool EthTransactionValidator::_verify_transaction_signature(
    EthTxMessage tx_msg
) const
{
    try {
        utils::crypto::Signature signature;
        signature.encode_signature(tx_msg.v(), tx_msg.r(), tx_msg.s());
        std::vector<uint8_t> unsigned_msg = tx_msg.get_unsigned_msg();
        Sha256_t msg_hash = utils::crypto::keccak_sha3(unsigned_msg.data(), 0, unsigned_msg.size());
        std::vector<uint8_t> public_key = signature.recover(msg_hash);

        return signature.verify(public_key, std::move(unsigned_msg));
    } catch (...) {
        // TODO catch a better exception
        return false;
    }
}

bool EthTransactionValidator::_parse_transaction(
    const PTxContents_t& txs_message_contents, EthTxMessage_t& tx_msg
) const
{
    try {
        EthTxMessage_t msg;
        msg.decode(*txs_message_contents, 0);
        if ( ! msg.deserialize() ) {
            return false;
        }
        tx_msg = EthTxMessage_t(std::move(msg));
        return true;
    } catch (...) {
        // TODO catch a better exception
        return false;
    }
}

size_t EthTransactionValidator::transaction_validation (
    const PTxContents_t& txs_message_contents,
    const uint64_t min_tx_network_fee
) const
{
    EthTxMessage_t tx_msg;
    if ( _parse_transaction(txs_message_contents, tx_msg) ) {
        if (_verify_transaction_signature(tx_msg)) {
            if (tx_msg.gas_price() >= min_tx_network_fee) {
                return TX_VALIDATION_STATUS_VALID_TX;
            }
            else {
                return TX_VALIDATION_STATUS_INVALID_LOW_FEE;
            }
        } else {
            return TX_VALIDATION_STATUS_INVALID_SIGNATURE;
        }
    }

    return TX_VALIDATION_STATUS_INVALID_FORMAT;
}

} // ethereum
} // protocols
} // utils
