#include <climits>
#include <assert.h>

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
    Rlp_t rlp(msg_buf,0, 0, offset);
//    _r.clear();
//    _s.clear();
//    _address.clear();
    _rlp_list = rlp.get_rlp_list();
    if (_rlp_list.size() < S_IDX) {
        return from;
    }

    _nonce = _rlp_list[NONCE_IDX].as_int();
    _gas_price = _rlp_list[GAS_PRICE_IDX].as_int();
    _start_gas = _rlp_list[START_GAS_IDX].as_int();
    _address = _rlp_list[TO_IDX].as_vector();
    _value = _rlp_list[VALUE_IDX].as_large_int();
    _data = _rlp_list[DATA_IDX].as_rlp_string();
    _v = _rlp_list[V_IDX].as_int();
    _r = _rlp_list[R_IDX].as_vector();
    _s = _rlp_list[S_IDX].as_vector();

    offset += rlp.val_offset() + rlp.length();
    _sha = std::move(crypto::keccak_sha3(msg_buf.byte_array(), from, offset - from));
    return offset;
}

bool EthTxMessage::deserialize() {
    bool deserialize_status = true;
    try {
        if (
            ! _deserialize_string(_rlp_list[DATA_IDX].length(), ULLONG_MAX)
            or ! _deserialize_string(address().size(), ETH_ADDRESS_LEN)
            ) {
            return false;
        }

        std::vector<int> int_indexes{NONCE_IDX, GAS_PRICE_IDX, START_GAS_IDX, VALUE_IDX, V_IDX, R_IDX, S_IDX};
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
    uint64_t unsigned_msg_starting_offset = _rlp_list[NONCE_IDX].rlp_starting_offset();
    uint64_t unsigned_msg_ending_offset =
        _rlp_list[DATA_IDX].rlp_starting_offset() + _rlp_list[DATA_IDX].val_offset() + _rlp_list[DATA_IDX].length();
    uint64_t unsigned_msg_len = unsigned_msg_ending_offset - unsigned_msg_starting_offset;

    std::vector<uint8_t> unsigned_msg;
    size_t rlp_len = 0;

    if ( _v >= EIP155_CHAIN_ID_OFFSET ) {
        rlp_len = utils::encoding::get_length_prefix_list(
            unsigned_msg, unsigned_msg_len + 3, 0
        );
        unsigned_msg.resize(rlp_len + unsigned_msg_len + 3);
        if ( _v % 2 == 0 ) {
            unsigned_msg[rlp_len + unsigned_msg_len] = V_RANGE_END;
        } else {
            unsigned_msg[rlp_len + unsigned_msg_len] = V_RANGE_START;
        }

        unsigned_msg[rlp_len + unsigned_msg_len + 1] = 0x80; // = r
        unsigned_msg[rlp_len + unsigned_msg_len + 2] = 0x80; // = s
    } else {
        rlp_len = utils::encoding::get_length_prefix_list(
            unsigned_msg, unsigned_msg_len, 0
        );
        unsigned_msg.resize(rlp_len + unsigned_msg_len);
    }

    memcpy(
        &unsigned_msg[rlp_len],
        &_rlp_list[NONCE_IDX].payload().at(unsigned_msg_starting_offset),
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

uint64_t EthTxMessage::start_gas() const {
    return _start_gas;
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

const Address_t EthTxMessage::address() const {
    return _address;
}

const BufferView_t& EthTxMessage::data() const {
    return _data;
}

const Sha256_t& EthTxMessage::hash() const {
    return _sha;
}



} // ethereum
} // protocols
} // utils