#include <iostream>
#include <jsoncons/json.hpp>

#ifndef UTILS_ENCODING_JSON_ENCODER_H_
#define UTILS_ENCODING_JSON_ENCODER_H_

namespace utils {
namespace encoding {

template <class TJsonSerializable, class TStringInput>
TJsonSerializable decode_json(const TStringInput& json_str) {
    return jsoncons::decode_json<TJsonSerializable>(json_str);
}

template <class TJsonSerializable>
std::string encode_json(const TJsonSerializable& json_obj) {
    std::string json_str;
    jsoncons::encode_json<TJsonSerializable>(json_obj, json_str);
    return json_str;
}

} // encoding
} // utils

#endif //UTILS_ENCODING_JSON_ENCODER_H_
