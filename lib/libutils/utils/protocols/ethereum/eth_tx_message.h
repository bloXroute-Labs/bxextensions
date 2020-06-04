#include <iostream>
#include <array>

#include "utils/crypto/sha256.h"
#include "utils/common/buffer_view.h"

#define ETH_ADDRESS_LEN 20

#ifndef UTILS_PROTOCOLS_ETHEREUM_ETH_TX_MESSAGE_H_
#define UTILS_PROTOCOLS_ETHEREUM_ETH_TX_MESSAGE_H_

namespace utils {
namespace protocols {
namespace ethereum {

typedef common::BufferView BufferView_t;
typedef crypto::Sha256 Sha256_t;
typedef std::array<uint8_t, ETH_ADDRESS_LEN> Address_t;

class EthTxMessage {
public:
    size_t decode(const BufferView_t& msg_buf, size_t offset);

    uint64_t nonce() const;
    uint64_t gas_price() const;
    uint64_t start_gas() const;
    uint64_t value() const;
    uint64_t v() const;
    uint64_t r() const;
    uint64_t s() const;

    Address_t address() const;

    const BufferView_t& data() const;

    const Sha256_t& hash() const;

private:
    uint64_t _nonce, _gas_price, _start_gas, _value, _v, _r, _s;
    Address_t _address;
    BufferView_t _data;
    Sha256_t _sha;
};

} // ethereum
} // protocols
} // utils

#endif // UTILS_PROTOCOLS_ETHEREUM_ETH_TX_MESSAGE_H_
