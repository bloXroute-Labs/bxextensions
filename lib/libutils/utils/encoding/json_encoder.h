#include <iostream>
#include <jsoncons/json.hpp>

#ifndef UTILS_ENCODING_JSON_ENCODER_H_
#define UTILS_ENCODING_JSON_ENCODER_H_

namespace utils {
namespace encoding {

template <class TJsonSerializable, class TStringInput>
TJsonSerializable decode_json(const TStringInput& json_str) {
    jsoncons::ojson j = jsoncons::ojson::parse(json_str);
    return TJsonSerializable(j);
}

template <class TJsonSerializable>
std::string encode_json(const TJsonSerializable& json_obj) {
    std::string json_str;
    json_obj.to_json().dump(json_str);
    return std::move(json_str);
}

} // encoding
} // utils

#endif //UTILS_ENCODING_JSON_ENCODER_H_
