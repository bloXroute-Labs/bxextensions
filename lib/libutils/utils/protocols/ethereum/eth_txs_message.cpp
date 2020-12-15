#include "utils/protocols/ethereum/eth_txs_message.h"
#include "utils/encoding/rlp_encoder.h"

namespace utils {
namespace protocols {
namespace ethereum {


size_t EthTxsMessage::decode(const BufferView_t& msg_buf, size_t offset) {
    uint64_t length;
    offset = encoding::consume_length_prefix(msg_buf, length, offset);
    size_t end = offset + length;
    while (offset < end) {
        EthTxMessage tx;
        offset = tx.decode(msg_buf, offset);
        if (offset == 0) {
            throw std::runtime_error("bad transaction RLP!"); // TODO: throw proper exception here.
        }
        _transactions.push_back(std::move(tx));
    }
    return end;
}

const Transactions_t& EthTxsMessage::transactions() const {
    return _transactions;
}

} // ethereum
} // protocols
} // utils