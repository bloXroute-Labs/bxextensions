#pragma once

#include <utils/protocols/abstract_message_parser.h>

namespace utils {
namespace protocols {
namespace ontology {

typedef utils::protocols::AbstractMessageParser AbstractMessageParser_t;
typedef utils::protocols::ParsedTransactions_t ParsedTransactions_t;
typedef utils::protocols::PTxsMessageContents_t PTxsMessageContents_t;

class OntMessageParser: public AbstractMessageParser_t {
public:
    ParsedTransactions_t parse_transactions_message(PTxsMessageContents_t txs_message_contents) const;
};

}
}
}