#include "eth_transaction_validator.h"
#include <utils/crypto/signature.h>
#include "utils/common/string_helper.h"

namespace utils {
namespace protocols {
namespace ethereum {

bool EthTransactionValidator::_verify_transaction_signature(EthTxMessage tx_msg, Sha256_t& sender_nonce_key) const
{
    try {
        utils::crypto::Signature signature;
        char payload_type = tx_msg.payload_type();
        signature.encode_signature(tx_msg.v(), tx_msg.r(), tx_msg.s(), payload_type, tx_msg.y_parity());
        std::vector<uint8_t> unsigned_msg = tx_msg.get_unsigned_msg();
        Sha256_t msg_hash = utils::crypto::keccak_sha3(unsigned_msg.data(), 0, unsigned_msg.size());
        std::vector<uint8_t> public_key = signature.recover(msg_hash);
        Sha256_t address = utils::crypto::keccak_sha3(public_key.data(), 0, public_key.size());
        sender_nonce_key = Sha256_t(address, tx_msg.nonce());

        return signature.verify(public_key, unsigned_msg);
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
        if (tx_msg.decode(*txs_message_contents, 0)) {
            if (!tx_msg.deserialize()) {
                return false;
            }
            return true;
        }
        return false;
    } catch (...) {
        // TODO catch a better exception
        return false;
    }
}

TxValidationStatus_t EthTransactionValidator::transaction_validation (
    const PTxContents_t txs_message_contents,
    const uint64_t min_tx_network_fee,
    const double current_time,
    SenderNonceMap_t& sender_nonce_map,
    const double age,
    const double factor
) const
{
    EthTxMessage_t tx_msg;
    Sha256_t sender_nonce;
    if ( _parse_transaction(txs_message_contents, tx_msg) ) {
        if (_verify_transaction_signature(tx_msg, sender_nonce)) {
            uint64_t curr_gas_price = tx_msg.gas_price();
            if (tx_msg.gas_price() >= min_tx_network_fee) {
                if (current_time > 0.0) {
                    auto iter = sender_nonce_map.find(sender_nonce);
                    if (iter != sender_nonce_map.end()) {
                        double elem_time = iter->second.first;
                        uint64_t elem_gas_price = iter->second.second;
                        if (current_time < elem_time  and curr_gas_price < elem_gas_price) {
                            return TX_VALIDATION_STATUS_REUSE_SENDER_NONCE;
                        }
                    }

                    sender_nonce_map.emplace(
                        sender_nonce,
                        std::pair<double, uint64_t>(
                            current_time + age,
                            (uint64_t)(curr_gas_price *factor)
                            )
                    );
                }
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
