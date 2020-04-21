#include <iostream>
#include <jsoncons/json.hpp>

#ifndef UTILS_PROTOCOLS_ONTOLOGY_CONSENSUS_CONSENSUS_ONT_JSON_PAYLOAD_H_
#define UTILS_PROTOCOLS_ONTOLOGY_CONSENSUS_CONSENSUS_ONT_JSON_PAYLOAD_H_

namespace utils {
namespace protocols {
namespace ontology {
namespace consensus {

class ConsensusOntJsonPayload {
public:

    ConsensusOntJsonPayload(): _type(0), _len(0), _payload("") {

    }

    ConsensusOntJsonPayload(uint8_t type, size_t len, const std::string& payload):
        _type(type), _len(len), _payload(payload)
    {
    }

    bool operator ==(const ConsensusOntJsonPayload& other) const {
        return type() == other.type() && len() == other.len() && payload() == other.payload();
    }

    uint8_t type() const {
        return _type;
    }

    size_t len() const {
        return _len;
    }

    const std::string& payload() const {
        return _payload;
    }

private:
    uint8_t _type;
    size_t _len;
    std::string _payload;
};

} // consensus
} // ontology
} // protocols
} // utils

JSONCONS_ALL_GETTER_CTOR_TRAITS(
        utils::protocols::ontology::consensus::ConsensusOntJsonPayload, type, len, payload
)

#endif //UTILS_PROTOCOLS_ONTOLOGY_CONSENSUS_CONSENSUS_ONT_JSON_PAYLOAD_H_
