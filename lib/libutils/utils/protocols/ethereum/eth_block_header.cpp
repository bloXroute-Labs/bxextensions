#include <array>
#include <sstream>

#include "utils/protocols/ethereum/eth_block_header.h"
#include "utils/encoding/rlp_encoder.h"
#include "utils/crypto/keccak.h"
#include "utils/common/buffer_view.h"
#include "utils/common/string_helper.h"

namespace utils {
namespace protocols {
namespace ethereum {

typedef encoding::RlpList RlpList_t;

EthBlockHeader::EthBlockHeader(BufferView_t buffer){
    Rlp_t rlp(buffer, 0);

    RlpList_t rlp_list = rlp.get_rlp_list();
    _prev_hash = crypto::Sha256(rlp_list[0].as_rlp_string());
    _uncles_hash = crypto::Sha256(rlp_list[1].as_rlp_string());
    _coinbase = std::vector<uint8_t>(rlp_list[2].as_vector());
    _state_root = std::vector<uint8_t>(rlp_list[3].as_vector());
    _transaction_root = std::vector<uint8_t>(rlp_list[4].as_vector());
    _receipt_root = std::vector<uint8_t>(rlp_list[5].as_vector());
    _bloom = std::vector<uint8_t>(rlp_list[6].as_vector());
    _difficulty = rlp_list[7].as_int();
    _block_number = rlp_list[8].as_int();
    _gas_limit = rlp_list[9].as_int();
    _gas_used = rlp_list[10].as_int();
    _timestamp = rlp_list[11].as_int();
    _extra_data = std::vector<uint8_t>(rlp_list[12].as_vector());
    _mix_hash = crypto::Sha256(rlp_list[13].as_rlp_string());
    _nonce = rlp_list[14].as_int();
}

std::string EthBlockHeader::to_json() {
    std::stringstream ss;

    //put arbitrary formatted data into the stream
    ss << "\"header\":{" <<
        "\"parentHash\":\"0x" << _prev_hash.hex_string() << "\"," <<
        "\"sha3Uncles\":\"0x" << _uncles_hash.hex_string() << "\"," <<
        "\"miner\":\"0x" << utils::common::to_hex_string(_coinbase) << "\"," <<
        "\"stateRoot\":\"0x" << utils::common::to_hex_string(_state_root) << "\"," <<
        "\"transactionsRoot\":\"0x" << utils::common::to_hex_string(_transaction_root) << "\"," <<
        "\"receiptsRoot\":\"0x" << utils::common::to_hex_string(_receipt_root) << "\"," <<
        "\"logsBloom\":0x" << utils::common::to_hex_string(_bloom) << "\"," <<
        "\"difficulty\":0x" << std::hex << _difficulty << "\"," <<
        "\"number\":0x" << std::hex << _block_number << "\"," <<
        "\"gasLimit\":0x" << std::hex << _gas_limit << "\"," <<
        "\"gasUsed\":0x" << std::hex << _gas_used << "\"," <<
        "\"timestamp\":0x" << std::hex << _timestamp << "\"," <<
        "\"extraData\":\"0x" << utils::common::to_hex_string(_extra_data) << "\"," <<
        "\"mixHash\":\"0x" << _mix_hash.hex_string() << "\"," <<
        "\"nonce\":0x" << std::hex << _nonce << "\"," <<
        "\"baseFeePerGas\":0x" << std::hex << _base_fee_per_gas << "\"" <<
        "}";

    return ss.str();
}


} // ethereum
} // protocols
} // utils
