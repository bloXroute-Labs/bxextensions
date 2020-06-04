#pragma once

#include <utils/protocols/abstract_message_converter.h>

namespace utils {
namespace protocols {
namespace ethereum {

typedef utils::protocols::AbstractMessageConverter AbstractMessageConverter_t;
typedef utils::protocols::ParsedTransactions_t ParsedTransactions_t;
typedef utils::protocols::PTxsMessageContents_t PTxsMessageContents_t;

class EthMessageConverter: public AbstractMessageConverter_t {
public:
    ParsedTransactions_t tx_to_bx_txs(PTxsMessageContents_t txs_message_contents) const;
};

}
}
}