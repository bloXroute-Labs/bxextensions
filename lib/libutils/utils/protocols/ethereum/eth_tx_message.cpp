#include <climits>

#include "utils/protocols/ethereum/eth_tx_message.h"
#include <utils/encoding/rlp_encoder.h>
#include <utils/protocols/ethereum/eth_consts.h>
#include <utils/crypto/keccak.h>


namespace utils {
namespace protocols {
namespace ethereum {

typedef utils::common::ByteArray ByteArray_t;

size_t EthTxMessage::decode(const BufferView_t& msg_buf, size_t offset) {
    const size_t from = offset;
    Rlp_t rlp;
    if (msg_buf.at(from) >= START_RANGE_ETH_LEGACY_TX and msg_buf.at(from) <= END_RANGE_ETH_LEGACY_TX) {
        // legacy ETH transaction - https://eips.ethereum.org/EIPS/eip-2718
        _payload_type = ETH_TX_TYPE_0;
    }
    else if (msg_buf.at(from) == ETH_TX_TYPE_0) {
        // New Type transaction, need to skip the first byte
        _payload_type = ETH_TX_TYPE_0;
        offset++;
    }
    else if (msg_buf.at(from) == ETH_TX_TYPE_1) {
        // New Type transaction, need to skip the first byte
        // https://eips.ethereum.org/EIPS/eip-2930
        _payload_type = ETH_TX_TYPE_1;
        offset++;
    }
    else {
        throw std::runtime_error("ETH Transaction is not valid");
    }

    rlp = Rlp_t(msg_buf, 0, 0, offset);

    _rlp_list = rlp.get_rlp_list();
    if (_payload_type == ETH_TX_TYPE_0) {
        if (_rlp_list.size() < ETH_TYPE_0_S_IDX) {
            // this is a receipt message, we should not handle it
            return from;
        }

        _nonce = _rlp_list[ETH_TYPE_0_NONCE_IDX].as_int();
        _gas_price = _rlp_list[ETH_TYPE_0_GAS_PRICE_IDX].as_int();
        _gas_limit = _rlp_list[ETH_TYPE_0_GAS_LIMIT_IDX].as_int();
        _address = _rlp_list[ETH_TYPE_0_TO_IDX].as_vector();
        _value = _rlp_list[ETH_TYPE_0_VALUE_IDX].as_large_int();
        _data = _rlp_list[ETH_TYPE_0_DATA_IDX].as_rlp_string();
        _v = _rlp_list[ETH_TYPE_0_V_IDX].as_int();
        _r = _rlp_list[ETH_TYPE_0_R_IDX].as_vector();
        _s = _rlp_list[ETH_TYPE_0_S_IDX].as_vector();

        uint8_t v = _v;
        if ((v % 2) == 0) {
            v -= 1;
        }
        _chain_id = uint8_t((v - EIP155_CHAIN_ID_OFFSET)/2);

        // New ETH items not in used in this transaction type
        _access_list = std::vector<uint8_t>(0);
        _y_parity = 0;
    }
    else if (_payload_type == ETH_TX_TYPE_1) {
        //  to, value, data, access_list, yParity, senderR, senderS
        _chain_id = uint8_t(_rlp_list[ETH_TYPE_1_CHAIN_ID_IDX].as_int());
        _nonce = _rlp_list[ETH_TYPE_1_NONCE_IDX].as_int();
        _gas_price = _rlp_list[ETH_TYPE_1_GAS_PRICE_IDX].as_int();
        _gas_limit = _rlp_list[ETH_TYPE_1_GAS_LIMIT_IDX].as_int();
        _address = _rlp_list[ETH_TYPE_1_TO_IDX].as_vector();
        _value = _rlp_list[ETH_TYPE_1_VALUE_IDX].as_large_int();
        _data = _rlp_list[ETH_TYPE_1_DATA_IDX].as_rlp_string();
        _access_list = _rlp_list[ETH_TYPE_1_ACCESS_LIST_IDX].as_vector();
        _y_parity = uint8_t(_rlp_list[ETH_TYPE_1_Y_PARITY_IDX].as_int());
        _r = _rlp_list[ETH_TYPE_1_SENDER_R_IDX].as_vector();
        _s = _rlp_list[ETH_TYPE_1_SENDER_S_IDX].as_vector();

        // Old ETH items not in used in this transaction type
        _v = 0;

    }
    offset += rlp.val_offset() + rlp.length();
    _sha = crypto::keccak_sha3(msg_buf.byte_array(), from, offset - from);
    return offset;
}

bool EthTxMessage::deserialize() {
    bool deserialize_status = true;
    try {
        if (_payload_type == ETH_TX_TYPE_0) {
            if (
                !_deserialize_string(_rlp_list[ETH_TYPE_0_DATA_IDX].length(), ULLONG_MAX)
                or !_deserialize_string(address().size(), ETH_ADDRESS_LEN)
                ) {
                return false;
            }
        } else if (_payload_type == ETH_TX_TYPE_1) {
            if (
                !_deserialize_string(_rlp_list[ETH_TYPE_1_DATA_IDX].length(), ULLONG_MAX)
                or !_deserialize_string(address().size(), ETH_ADDRESS_LEN)
                ) {
                return false;
            }

        }

        std::vector<int> int_indexes(0);
        if (_payload_type == ETH_TX_TYPE_0) {
            int_indexes = {ETH_TYPE_0_NONCE_IDX, ETH_TYPE_0_GAS_PRICE_IDX, ETH_TYPE_0_GAS_LIMIT_IDX,
                           ETH_TYPE_0_VALUE_IDX, ETH_TYPE_0_V_IDX, ETH_TYPE_0_R_IDX, ETH_TYPE_0_S_IDX};
        }
        else if (_payload_type == ETH_TX_TYPE_1) {
            int_indexes = {ETH_TYPE_1_CHAIN_ID_IDX, ETH_TYPE_1_NONCE_IDX, ETH_TYPE_1_GAS_PRICE_IDX, ETH_TYPE_1_GAS_LIMIT_IDX,
                           ETH_TYPE_1_VALUE_IDX, ETH_TYPE_1_Y_PARITY_IDX, ETH_TYPE_1_SENDER_R_IDX, ETH_TYPE_1_SENDER_S_IDX};
        }

        for (auto idx: int_indexes) {
            if ( ! _deserialize_int(_rlp_list[idx]) ) {
                return false;
            }
        }
    } catch ( ... ) {
        deserialize_status = false;
    }
    return deserialize_status;
}

std::vector<uint8_t> EthTxMessage::get_unsigned_msg() {
    uint64_t unsigned_msg_starting_offset;
    uint64_t unsigned_msg_ending_offset;
    uint64_t unsigned_msg_len;
    int first_item_idx, last_item_idx;

    std::vector<uint8_t> unsigned_msg;
    size_t rlp_len = 0;

    if (_payload_type == ETH_TX_TYPE_0) {
        first_item_idx = ETH_TYPE_0_NONCE_IDX;
        last_item_idx = ETH_TYPE_0_DATA_IDX;
    }
    if (_payload_type == ETH_TX_TYPE_1) {
        first_item_idx = ETH_TYPE_1_CHAIN_ID_IDX;
        last_item_idx = ETH_TYPE_1_ACCESS_LIST_IDX;
    }

    unsigned_msg_starting_offset = _rlp_list[first_item_idx].rlp_starting_offset();
    unsigned_msg_ending_offset =
        _rlp_list[last_item_idx].rlp_starting_offset() + _rlp_list[last_item_idx].val_offset() +
        _rlp_list[last_item_idx].length();
    unsigned_msg_len = unsigned_msg_ending_offset - unsigned_msg_starting_offset;

    if (_payload_type == ETH_TX_TYPE_0) {
        if (_v >= EIP155_CHAIN_ID_OFFSET) {
            rlp_len = utils::encoding::get_length_prefix_list(
                unsigned_msg, unsigned_msg_len + 3, 0
            );
            unsigned_msg.resize(rlp_len + unsigned_msg_len + 3);
            unsigned_msg[rlp_len + unsigned_msg_len] = _chain_id;
            unsigned_msg[rlp_len + unsigned_msg_len + 1] = 0x80; // = r
            unsigned_msg[rlp_len + unsigned_msg_len + 2] = 0x80; // = s
        } else {
            rlp_len = utils::encoding::get_length_prefix_list(
                unsigned_msg, unsigned_msg_len, 0
            );
            unsigned_msg.resize(rlp_len + unsigned_msg_len);
        }
    } else if (_payload_type == ETH_TX_TYPE_1) {
        rlp_len = utils::encoding::get_length_prefix_list(
            unsigned_msg, unsigned_msg_len, 1
        );
        unsigned_msg.resize(rlp_len + unsigned_msg_len);
        unsigned_msg[0] = _payload_type;
    }

    memcpy(
        &unsigned_msg[rlp_len],
        &_rlp_list[first_item_idx].payload().at(unsigned_msg_starting_offset),
        unsigned_msg_len
    );

    return unsigned_msg;
}

bool EthTxMessage::_deserialize_int(Rlp_t& rlp) {
    try {
        uint64_t length = 0;
        if (rlp.length() > sizeof(uint64_t) ) {
            std::vector<uint64_t> values;
            utils::encoding::get_big_endian_rlp_large_value(
                rlp.payload(),
                length,
                rlp.rlp_starting_offset() + rlp.val_offset(),
                rlp.length(),
                values
            );
        } else {
            utils::encoding::get_big_endian_rlp_value(
                rlp.payload(),
                length,
                rlp.rlp_starting_offset() + rlp.val_offset(),
                rlp.length()
            );
        }
        return true;
    } catch ( ... ) {
        return false;
    }
}

bool EthTxMessage::_deserialize_string(size_t string_len, size_t max_length) {
    return (string_len <= max_length);
}

uint64_t EthTxMessage::nonce() const {
    return _nonce;
}

uint64_t EthTxMessage::gas_price() const {
    return _gas_price;
}

uint64_t EthTxMessage::gas_limit() const {
    return _gas_limit;
}

const std::vector<uint64_t>& EthTxMessage::value() const {
    return _value;
}

uint64_t EthTxMessage::v() const {
    return _v;
}

const std::vector<uint8_t>& EthTxMessage::r() const {
    return _r;
}

const std::vector<uint8_t>& EthTxMessage::s() const {
    return _s;
}

uint8_t EthTxMessage::y_parity() const {
    return _y_parity;
};

const std::vector<uint8_t>& EthTxMessage::access_list() const {
    return _access_list;
};

const Address_t EthTxMessage::address() const {
    return _address;
}

const BufferView_t& EthTxMessage::data() const {
    return _data;
}

const Sha256_t& EthTxMessage::hash() const {
    return _sha;
}

char EthTxMessage::payload_type() const {
    return _payload_type;
};


} // ethereum
} // protocols
} // utils