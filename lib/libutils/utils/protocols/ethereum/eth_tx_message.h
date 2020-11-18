#include <iostream>
#include <vector>

#include "utils/crypto/sha256.h"
#include "utils/common/buffer_view.h"
#include <utils/encoding/rlp.h>
#include <utils/common/tx_status_consts.h>

#define ETH_ADDRESS_LEN 20
#define NONCE_IDX 0
#define GAS_PRICE_IDX 1
#define START_GAS_IDX 2
#define TO_IDX 3
#define VALUE_IDX 4
#define DATA_IDX 5
#define V_IDX 6
#define R_IDX 7
#define S_IDX 8

#ifndef UTILS_PROTOCOLS_ETHEREUM_ETH_TX_MESSAGE_H_
#define UTILS_PROTOCOLS_ETHEREUM_ETH_TX_MESSAGE_H_

namespace utils {
namespace protocols {
namespace ethereum {

typedef common::BufferView BufferView_t;
typedef crypto::Sha256 Sha256_t;
typedef std::vector<uint8_t> Address_t;
typedef encoding::Rlp Rlp_t;
typedef encoding::RlpList RlpList_t;

class EthTxMessage {
protected:
    bool _deserialize_int(Rlp_t&);
    bool _deserialize_string(size_t, size_t);
public:
    size_t decode(const BufferView_t& msg_buf, size_t offset);
    bool deserialize();
    std::vector<uint8_t> get_unsigned_msg();

    uint64_t nonce() const;
    uint64_t gas_price() const;
    uint64_t start_gas() const;
    const std::vector<uint64_t>& value() const;
    uint64_t v() const;
    const std::vector<uint8_t>& r() const;
    const std::vector<uint8_t>& s() const;
    const Address_t address() const;
    const BufferView_t& data() const;
    const Sha256_t& hash() const;

private:
    uint64_t _nonce, _gas_price, _start_gas, _v;
    std::vector<uint8_t> _r, _s;
    std::vector<uint64_t> _value;
    Address_t _address;
    BufferView_t _data;
    Sha256_t _sha;
    RlpList_t _rlp_list;
};

} // ethereum
} // protocols
} // utils

#endif // UTILS_PROTOCOLS_ETHEREUM_ETH_TX_MESSAGE_H_
