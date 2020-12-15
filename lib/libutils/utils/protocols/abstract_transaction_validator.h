#pragma once

#include <utils/common/byte_array.h>
#include <memory>

namespace utils {
namespace protocols {

typedef utils::common::BufferView TxContents_t;
typedef std::shared_ptr<TxContents_t> PTxContents_t;


class AbstractTransactionValidator {
public:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
    virtual size_t transaction_validation(
        const PTxContents_t& tx,
        const uint64_t min_tx_network_fee
    ) const {
        throw std::runtime_error("transaction_validation is not implemented in abstract class.");
    }
};
#pragma GCC diagnostic pop

} // protocols
} // utils
