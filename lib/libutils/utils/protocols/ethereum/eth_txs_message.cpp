#include "utils/protocols/ethereum/eth_txs_message.h"
#include "utils/encoding/rlp_encoder.h"

namespace utils {
namespace protocols {
namespace ethereum {


size_t EthTxsMessage::decode(const BufferView_t& msg_buf, size_t offset) {
    uint64_t length;
    uint8_t rlp_type;

    offset = encoding::consume_length_prefix(msg_buf, length, rlp_type, offset);
    size_t _offset = 0;
    size_t end = offset + length;
    while (offset < end) {
        EthTxMessage tx;
        _offset = tx.decode(msg_buf, offset);
        if (_offset == offset) {
            throw std::runtime_error("bad transaction RLP!"); // TODO: throw proper exception here.
        }
        _transactions.push_back(std::move(tx));
        offset = _offset;
    }
    return end;
}

const Transactions_t& EthTxsMessage::transactions() const {
    return _transactions;
}

} // ethereum
} // protocols
} // utils