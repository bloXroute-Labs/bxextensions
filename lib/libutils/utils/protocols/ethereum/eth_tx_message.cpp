#include <climits>

#include "utils/protocols/ethereum/eth_tx_message.h"
#include "utils/encoding/rlp_encoder.h"
#include "utils/protocols/ethereum/eth_consts.h"
#include "utils/crypto/keccak.h"
#include "utils/common/string_helper.h"


namespace utils {
namespace protocols {
namespace ethereum {

typedef utils::common::ByteArray ByteArray_t;

size_t EthTxMessage::decode(const BufferView_t& msg_buf, size_t offset) {
    const size_t from = offset;
    Rlp_t rlp(msg_buf, offset);

    size_t starting_offset_for_hash = offset;
    size_t payload_len_for_hash = rlp.length() + rlp.val_offset() - rlp.rlp_starting_offset();
    if (rlp.rlp_type() == RLP_LIST) {
        _payload_type = ETH_TX_TYPE_0;
    }
    else {
        if (rlp.rlp_type() == RLP_STRING) {
            starting_offset_for_hash = rlp.val_offset() ;
            if (rlp.length() < 1) {
                throw std::runtime_error("ETH Transaction is not valid");
            }
        }
        uint8_t tx_type = rlp.payload().at(starting_offset_for_hash);
        if (tx_type == ETH_TX_TYPE_0) {
            // for legacy transactions that appear as rlp_string, need to skip the extra byte and not include it in the hash calcuation
            _payload_type = ETH_TX_TYPE_0;
            starting_offset_for_hash++;
        } else if (tx_type <= ETH_TX_TYPE_2) {
            _payload_type = tx_type;
        } else {
            throw std::runtime_error("ETH Transaction Type is not valid");
        }
        rlp = Rlp_t(msg_buf, rlp.val_offset() + 1);
        payload_len_for_hash = rlp.length() + rlp.val_offset() - rlp.rlp_starting_offset() + 1;
    }

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
    } else if (_payload_type == ETH_TX_TYPE_1) {
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
    } else if (_payload_type == ETH_TX_TYPE_2) {
        //  max_priority_fee_per_gas, max_fee_per_gas, start_gas, to, value, data, access_list, yParity, senderR, senderS
        _chain_id = uint8_t(_rlp_list[ETH_TYPE_2_CHAIN_ID_IDX].as_int());
        _nonce = _rlp_list[ETH_TYPE_2_NONCE_IDX].as_int();
        _max_priority_fee_per_gas = _rlp_list[ETH_TYPE_2_MAX_PRIORITY_FEE_PER_GAS_IDX].as_int();
        _max_fee_per_gas = _rlp_list[ETH_TYPE_2_MAX_FEE_PER_GAS_IDX].as_int();
        _gas_price = _max_fee_per_gas;
        _gas_limit = 0;
        _start_gas = _rlp_list[ETH_TYPE_2_START_GAS_IDX].as_int();
        _address = _rlp_list[ETH_TYPE_2_TO_IDX].as_vector();
        _value = _rlp_list[ETH_TYPE_2_VALUE_IDX].as_large_int();
        _data = _rlp_list[ETH_TYPE_2_DATA_IDX].as_rlp_string();
        _access_list = _rlp_list[ETH_TYPE_2_ACCESS_LIST_IDX].as_vector();
        _y_parity = uint8_t(_rlp_list[ETH_TYPE_2_Y_PARITY_IDX].as_int());
        _r = _rlp_list[ETH_TYPE_2_SENDER_R_IDX].as_vector();
        _s = _rlp_list[ETH_TYPE_2_SENDER_S_IDX].as_vector();

        // Old ETH items not in used in this transaction type
        _v = 0;
    }

    _sha = crypto::keccak_sha3(msg_buf.byte_array(), starting_offset_for_hash, payload_len_for_hash);
    return rlp.val_offset() + rlp.length();
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
        } else if (_payload_type == ETH_TX_TYPE_2) {
            if (
                !_deserialize_string(_rlp_list[ETH_TYPE_2_DATA_IDX].length(), ULLONG_MAX)
                or !_deserialize_string(address().size(), ETH_ADDRESS_LEN)
                ) {
                return false;
            }
        }

        std::vector<int> int_indexes(0);
        if (_payload_type == ETH_TX_TYPE_0) {
            int_indexes = {
                ETH_TYPE_0_NONCE_IDX, ETH_TYPE_0_GAS_PRICE_IDX, ETH_TYPE_0_GAS_LIMIT_IDX,
                ETH_TYPE_0_VALUE_IDX, ETH_TYPE_0_V_IDX, ETH_TYPE_0_R_IDX, ETH_TYPE_0_S_IDX
            };
        } else if (_payload_type == ETH_TX_TYPE_1) {
            int_indexes = {
                ETH_TYPE_1_CHAIN_ID_IDX, ETH_TYPE_1_NONCE_IDX, ETH_TYPE_1_GAS_PRICE_IDX, ETH_TYPE_1_GAS_LIMIT_IDX,
                ETH_TYPE_1_VALUE_IDX, ETH_TYPE_1_Y_PARITY_IDX, ETH_TYPE_1_SENDER_R_IDX, ETH_TYPE_1_SENDER_S_IDX
            };
        } else if (_payload_type == ETH_TX_TYPE_2) {
            int_indexes = {
                ETH_TYPE_2_CHAIN_ID_IDX, ETH_TYPE_2_NONCE_IDX, ETH_TYPE_2_MAX_PRIORITY_FEE_PER_GAS_IDX,
                ETH_TYPE_2_MAX_FEE_PER_GAS_IDX, ETH_TYPE_2_START_GAS_IDX,
                ETH_TYPE_2_VALUE_IDX, ETH_TYPE_2_Y_PARITY_IDX, ETH_TYPE_2_SENDER_R_IDX, ETH_TYPE_2_SENDER_S_IDX
            };
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

    std::vector<uint8_t> unsigned_msg;
    size_t rlp_len = 0;

    int first_item_idx = ETH_TYPE_0_NONCE_IDX;
    int last_item_idx = ETH_TYPE_0_DATA_IDX;

    if (_payload_type == ETH_TX_TYPE_1) {
        first_item_idx = ETH_TYPE_1_CHAIN_ID_IDX;
        last_item_idx = ETH_TYPE_1_ACCESS_LIST_IDX;
    } else if (_payload_type == ETH_TX_TYPE_2) {
        first_item_idx = ETH_TYPE_2_CHAIN_ID_IDX;
        last_item_idx = ETH_TYPE_2_ACCESS_LIST_IDX;
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
    } else if (_payload_type <= ETH_TX_TYPE_2) {
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

std::string EthTxMessage::to_json() {
    std::stringstream ss;

    //put arbitrary formatted data into the stream
    ss << "{" <<
        "\"type\":\"0x" << std::hex << std::setw(2) << std::setfill('0') << (short)_payload_type << "\"," <<
        "\"hash\":\"0x" << _sha.hex_string() << "\",";

    if (_payload_type == ETH_TX_TYPE_0) {
        ss <<
            "\"nonce\":\"0x" << std::hex << std::setw(2) << std::setfill('0') << _nonce << "\"," <<
            "\"gasPrice\":\"0x" << std::hex << std::setw(2) << std::setfill('0') << _gas_price << "\"," <<
            "\"gas\":\"0x" << std::hex << std::setw(2) << std::setfill('0') << _gas_limit << "\"," <<
            "\"to\":\"0x" << utils::common::to_hex_string(_address) << "\"," <<
            "\"value\":\"0x" << utils::common::to_hex_string(_value) << "\"," <<
            "\"data\":\"0x" << utils::common::to_hex_string(_data.vector(_data.get_offset(), _data.size())) << "\"," <<
            "\"v\":\"0x" << std::hex << std::setw(2) << std::setfill('0') << (short)_v << "\"," <<
            "\"r\":\"0x" << utils::common::to_hex_string(_r) << "\"," <<
            "\"s\":\"0x" << utils::common::to_hex_string(_s) << "\"" <<
        "}";
    }
    else if (_payload_type == ETH_TX_TYPE_1) {
        ss <<
            "\"chainId\":\"0x" << std::hex << std::setw(2) << std::setfill('0') << (short)_chain_id << "\"," <<
            "\"nonce\":\"0x" << std::hex << std::setw(2) << std::setfill('0') << _nonce << "\"," <<
            "\"gasPrice\":\"0x" << std::hex << std::setw(2) << std::setfill('0') << _gas_price << "\"," <<
            "\"gas\":\"0x" << std::hex << std::setw(2) << std::setfill('0')<< _gas_limit << "\"," <<
            "\"to\":\"0x" << utils::common::to_hex_string(_address) << "\"," <<
            "\"value\":\"0x" << utils::common::to_hex_string(_value) << "\"," <<
            "\"data\":\"0x" << utils::common::to_hex_string(_data.vector(_data.get_offset(), _data.size())) << "\"," <<
            _access_list_json() <<
            "\"v\":\"0x" << std::hex << std::setw(2) << std::setfill('0') << (short)_y_parity << "\"," <<
            "\"r\":\"0x" << utils::common::to_hex_string(_r) << "\"," <<
            "\"s\":\"0x" << utils::common::to_hex_string(_s) << "\"" <<
            "}";
    }
    else if (_payload_type == ETH_TX_TYPE_2) {
        ss <<
            "\"chainId\":\"0x" << std::hex << std::setw(2) << std::setfill('0')<< (short)_chain_id << "\"," <<
            "\"nonce\":\"0x" << std::hex << std::setw(2) << std::setfill('0')<< _nonce << "\"," <<
            "\"maxPriorityFeePerGas\":\"0x" << std::hex << std::setw(2) << std::setfill('0') << _max_priority_fee_per_gas << "\"," <<
            "\"maxFeePerGas\":\"0x" << std::hex << std::setw(2) << std::setfill('0') << _max_fee_per_gas << "\"," <<
            "\"startGas\":\"0x" << std::hex << std::setw(2) << std::setfill('0') << _gas_limit << "\"," <<
            "\"to\":\"0x" << utils::common::to_hex_string(_address) << "\"," <<
            "\"value\":\"0x" << utils::common::to_hex_string(_value) << "\"," <<
            "\"data\":\"0x" << utils::common::to_hex_string(_data.vector(_data.get_offset(), _data.size())) << "\"," <<
            _access_list_json() <<
            "\"v\":\"0x" << std::hex << std::setw(2) << std::setfill('0') << (short)_y_parity << "\"," <<
            "\"r\":\"0x" << utils::common::to_hex_string(_r) << "\"," <<
            "\"s\":\"0x" << utils::common::to_hex_string(_s) << "\"" <<
            "}";
    }

    return ss.str();
}


uint64_t EthTxMessage::nonce() const {
    return _nonce;
}

uint64_t EthTxMessage::gas_price() const {
    return _gas_price;
}

uint64_t EthTxMessage::max_priority_fee_per_gas() const {
    return _max_priority_fee_per_gas;
}


uint64_t EthTxMessage::max_fee_per_gas() const {
    return _max_fee_per_gas;
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

uint8_t EthTxMessage::chain_id() const {
    return _chain_id;
}

std::string EthTxMessage::_access_list_json() {
    std::stringstream ss;

    ss << "\"accessList\":[";
    // parse access list
    if (_access_list.size() > 0) {
        size_t from = 0;
        size_t end = _access_list.size();
        BufferView_t access_list_bf(&_access_list.at(0), _access_list.size(), 0);
        while (from < end) {
            Rlp_t access_list_rlp(access_list_bf, from);
            RlpList_t access_list_rlp_list = access_list_rlp.get_rlp_list();
            // first element in an accesslist is string with fixed len == 20
            ss <<
            "{\"address\":\"0x" << utils::common::to_hex_string(access_list_rlp_list[0].as_rlp_string()) << "\"," <<
            "\"storageKeys\":[";

            //second element in an accesslist is a list of fixed strings == 32
            Rlp_t storage_rlp(access_list_bf, access_list_rlp_list[1].rlp_starting_offset());
            RlpList_t storage_rlp_list = storage_rlp.get_rlp_list();
            for (size_t i = 0; i < storage_rlp_list.size(); i++) {
                ss <<
                "\"0x" << utils::common::to_hex_string(storage_rlp_list[i].as_rlp_string()) << "\"";
                if (i < storage_rlp_list.size() -1) {
                    ss << ",";
                }
            }
            ss << "]}";
            from = access_list_rlp.length() + access_list_rlp.val_offset();
            if (from < end) {
                ss << ",";
            }
        }
    }
    ss << "]";

    return ss.str();
}

} // ethereum
} // protocols
} // utils
