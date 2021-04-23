#include <iostream>
#include <vector>

#include "utils/crypto/sha256.h"
#include "utils/common/buffer_view.h"
#include <utils/encoding/rlp.h>
#include <utils/common/tx_status_consts.h>

#define ETH_ADDRESS_LEN 20
#define ETH_TYPE_0_NONCE_IDX 0
#define ETH_TYPE_0_GAS_PRICE_IDX 1
#define ETH_TYPE_0_GAS_LIMIT_IDX 2
#define ETH_TYPE_0_TO_IDX 3
#define ETH_TYPE_0_VALUE_IDX 4
#define ETH_TYPE_0_DATA_IDX 5
#define ETH_TYPE_0_V_IDX 6
#define ETH_TYPE_0_R_IDX 7
#define ETH_TYPE_0_S_IDX 8

#define ETH_TYPE_1_CHAIN_ID_IDX     0
#define ETH_TYPE_1_NONCE_IDX        1
#define ETH_TYPE_1_GAS_PRICE_IDX    2
#define ETH_TYPE_1_GAS_LIMIT_IDX    3
#define ETH_TYPE_1_TO_IDX           4
#define ETH_TYPE_1_VALUE_IDX        5
#define ETH_TYPE_1_DATA_IDX         6
#define ETH_TYPE_1_ACCESS_LIST_IDX  7
#define ETH_TYPE_1_Y_PARITY_IDX     8
#define ETH_TYPE_1_SENDER_R_IDX     9
#define ETH_TYPE_1_SENDER_S_IDX     10

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
    uint64_t gas_limit() const;
    const std::vector<uint64_t>& value() const;
    uint64_t v() const;
    const std::vector<uint8_t>& r() const;
    const std::vector<uint8_t>& s() const;
    const Address_t address() const;
    const BufferView_t& data() const;
    const Sha256_t& hash() const;
    uint8_t y_parity() const;
    const std::vector<uint8_t>& access_list() const;
    char payload_type() const;

private:
    uint64_t _nonce, _gas_price, _gas_limit, _v;
    uint8_t _y_parity, _chain_id;
    std::vector<uint8_t> _r, _s;
    std::vector<uint64_t> _value;
    std::vector<uint8_t> _access_list;
    Address_t _address;
    BufferView_t _data;
    Sha256_t _sha;
    RlpList_t _rlp_list;
    char _payload_type;
    BufferView_t _tx;
};

} // ethereum
} // protocols
} // utils

#endif // UTILS_PROTOCOLS_ETHEREUM_ETH_TX_MESSAGE_H_
