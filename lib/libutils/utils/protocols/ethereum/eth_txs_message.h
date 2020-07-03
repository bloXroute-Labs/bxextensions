#include <iostream>

#include "utils/protocols/ethereum/eth_tx_message.h"

#ifndef UTILS_PROTOCOLS_ETHEREUM_ETH_TXS_MESSAGE_H_
#define UTILS_PROTOCOLS_ETHEREUM_ETH_TXS_MESSAGE_H_

namespace utils {
namespace protocols {
namespace ethereum {

typedef std::vector<EthTxMessage> Transactions_t;

class EthTxsMessage {
public:
    size_t decode(const BufferView_t& msg_buf, size_t offset);

    const Transactions_t& transactions() const;

private:
    Transactions_t _transactions;
};

} // ethereum
} // protocols
} // utils

#endif //UTILS_PROTOCOLS_ETHEREUM_ETH_TXS_MESSAGE_H_
