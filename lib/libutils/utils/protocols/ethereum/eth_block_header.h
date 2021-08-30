#include <iostream>
#include <utility>
#include <vector>
#include <cstdint>
#include <functional>

#include "utils/protocols/ethereum/eth_consts.h"
#include "utils/common/buffer_view.h"
#include "utils/common/byte_array.h"
#include "utils/crypto/sha256.h"
#include <utils/encoding/rlp.h>

#ifndef UTILS_PROTOCOLS_ethereum_ETH_BLOCK_HEADER_H_
#define UTILS_PROTOCOLS_ethereum_ETH_BLOCK_HEADER_H_

namespace utils {
namespace protocols {
namespace ethereum {

typedef common::BufferView BufferView_t;
typedef common::ByteArray ByteArray_t;
typedef crypto::Sha256 Sha256_t;
typedef encoding::Rlp Rlp_t;
typedef encoding::RlpList RlpList_t;

class EthBlockHeader {
public:
    EthBlockHeader(BufferView_t buffer);
    std::string to_json();
private:
    crypto::Sha256 _prev_hash, _uncles_hash, _mix_hash;

    std::vector<uint8_t> _coinbase;
    std::vector<uint8_t> _state_root, _transaction_root, _receipt_root;

    uint64_t _difficulty, _block_number, _gas_limit, _gas_used, _timestamp, _base_fee_per_gas, _nonce;
    std::vector<uint8_t> _extra_data;
    std::vector<uint8_t> _bloom;
};

} // ethereum
} // protocols
} // utils

#endif /* UTILS_PROTOCOLS_ethereum_ETH_BLOCK_HEADER_H_ */