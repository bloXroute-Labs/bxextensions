#include <iostream>
#include <jsoncons/json.hpp>

#ifndef UTILS_PROTOCOLS_ONTOLOGY_ONT_CONSENSUS_CONSENSUS_JSON_PAYLOAD_H_
#define UTILS_PROTOCOLS_ONTOLOGY_ONT_CONSENSUS_CONSENSUS_JSON_PAYLOAD_H_

namespace utils {
namespace protocols {
namespace ontology {
namespace consensus {

class OntConsensusJsonPayload {
public:

    OntConsensusJsonPayload(): _json() {
        _json.insert_or_assign("type", 0);
        _json.insert_or_assign("len", 0);
        _json.insert_or_assign("payload", "");
    }

    OntConsensusJsonPayload(uint8_t type, size_t len, const std::string& payload): _json() {
        _json.insert_or_assign("type", type);
        _json.insert_or_assign("len", len);
        _json.insert_or_assign("payload", payload);
    }

    OntConsensusJsonPayload(uint8_t type, size_t len, std::string&& payload): _json() {
        _json.insert_or_assign("type", type);
        _json.insert_or_assign("len", len);
        _json.insert_or_assign("payload", std::move(payload));
    }

    OntConsensusJsonPayload(const jsoncons::ojson& json):
        _json(json)
    {
    }

    OntConsensusJsonPayload(jsoncons::ojson&&  json): _json(std::move(json)) {
    }

    bool operator ==(const OntConsensusJsonPayload& other) const {
        return type() == other.type() && len() == other.len() && payload() == other.payload();
    }

    uint8_t type() const {
        return _json["type"].as<uint8_t>();
    }

    size_t len() const {
        return _json["len"].as<size_t>();
    }

    std::string payload() const {
        return _json["payload"].as_string();
    }

    const jsoncons::ojson& to_json() const {
        return _json;
    }

private:
    jsoncons::ojson _json;
};

} // consensus
} // ontology
} // protocols
} // utils

#endif //UTILS_PROTOCOLS_ONTOLOGY_ONT_CONSENSUS_CONSENSUS_JSON_PAYLOAD_H_
