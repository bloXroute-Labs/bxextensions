#include "utils/protocols/ethereum/eth_tx_message.h"
#include "utils/encoding/rlp.h"
#include "utils/crypto/keccak.h"

namespace utils {
namespace protocols {
namespace ethereum {

typedef encoding::Rlp Rlp_t;

size_t EthTxMessage::decode(const BufferView_t& msg_buf, size_t offset) {
    const size_t from = offset;
    Rlp_t rlp(msg_buf,0, offset);
    _nonce = rlp.get_next_rlp_string().as_int();
    _gas_price = rlp.get_next_rlp_string().as_int();
    _start_gas = rlp.get_next_rlp_string().as_int();
    auto to = rlp.get_next_rlp_string().as_rlp_string();
    if (to.size() == ETH_ADDRESS_LEN) {
        memcpy(_address.data(), to.byte_array(), ETH_ADDRESS_LEN);
    } else if (to.size() > 0) {
        throw std::runtime_error("Bad RLP"); // TODO: throw a proper exception here.
    }
    _value = rlp.get_next_rlp_string().as_int();
    _data = rlp.get_next_rlp_string().as_rlp_string();
    _v = rlp.get_next_rlp_string().as_int();
    _r = rlp.get_next_rlp_string().as_int();
    _s = rlp.get_next_rlp_string().as_int();
    offset = rlp.offset();
    _sha = std::move(crypto::keccak_sha3(msg_buf.byte_array(), from, offset - from));
    return offset;
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

uint64_t EthTxMessage::value() const {
    return _value;
}

uint64_t EthTxMessage::v() const {
    return _v;
}

uint64_t EthTxMessage::r() const {
    return _r;
}

uint64_t EthTxMessage::s() const {
    return _s;
}

Address_t EthTxMessage::address() const {
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