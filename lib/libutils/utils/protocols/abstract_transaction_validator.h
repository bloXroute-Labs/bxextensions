#pragma once

#include <utils/common/byte_array.h>
#include <utils/crypto/sha256.h>
#include <memory>

namespace utils {
namespace protocols {

typedef utils::common::BufferView TxContents_t;

typedef std::pair<double, uint64_t> SenderNonceVal_t;
typedef utils::crypto::Sha256OrderedMap_t<SenderNonceVal_t> SenderNonceMap_t;
typedef size_t TxValidationStatus_t;


class AbstractTransactionValidator {
public:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
    virtual TxValidationStatus_t transaction_validation(
        const TxContents_t& tx,
        const uint64_t min_tx_network_fee,
        const double current_time,
        SenderNonceMap_t& sender_nonce_map,
        const double age,
        const double factor
    ) const {
        throw std::runtime_error("transaction_validation is not implemented in abstract class.");
    }
};
#pragma GCC diagnostic pop

} // protocols
} // utils
