#pragma once

#include <utils/protocols/abstract_message_parser.h>

namespace utils {
namespace protocols {
namespace ethereum {

typedef utils::protocols::AbstractMessageParser AbstractMessageParser_t;
typedef utils::protocols::ParsedTransactions_t ParsedTransactions_t;
typedef utils::protocols::PTxsMessageContents_t PTxsMessageContents_t;

class EthMessageParser: public AbstractMessageParser_t {
public:
    ParsedTransactions_t parse_transactions_message(PTxsMessageContents_t txs_message_contents) const;
};

}
}
}