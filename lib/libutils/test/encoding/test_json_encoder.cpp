#include <gtest/gtest.h>
#include <iostream>

#include "utils/encoding/json_encoder.h"


class JsonSerializableTester {
public:

    JsonSerializableTester(int n, float f, double d, const std::string& str): _json() {
        _json.insert_or_assign("n", n);
        _json.insert_or_assign("f", f);
        _json.insert_or_assign("d", d);
        _json.insert_or_assign("str", str);
    }

    JsonSerializableTester(): _json() {
            _json.insert_or_assign("n", 0);
            _json.insert_or_assign("f", 0);
            _json.insert_or_assign("d", 0);
            _json.insert_or_assign("str", "");
    }

    JsonSerializableTester(const jsoncons::ojson& json): _json(json) {

    }

    JsonSerializableTester(jsoncons::ojson&& json): _json(std::move(json)) {

    }

    bool operator ==(const JsonSerializableTester& other) const {
        return n() == other.n() && f() == other.f() && d() == other.d() && str() == other.str();
    }

    int n() const {
        return _json["n"].as_integer();
    }

    float f() const {
        return _json["f"].as<float>();
    }

    double d() const {
        return _json["d"].as_double();
    }

    std::string str() const {
        return _json["str"].as_string();
    }

    const jsoncons::ojson& to_json() const {
        return _json;
    }

private:
    jsoncons::ojson _json;
};


class JsonSerializableTesterHolder {
public:

    JsonSerializableTesterHolder(const JsonSerializableTester& tester, const std::string& name): _json() {
        _json["tester"] = tester.to_json();
        _json.insert_or_assign("name", name);
    }

    JsonSerializableTesterHolder(const jsoncons::ojson& json): _json(json) {

    }

    JsonSerializableTesterHolder(jsoncons::ojson&& json): _json(std::move(json)) {

    }

    bool operator ==(const JsonSerializableTesterHolder& other) const {
        return tester() == other.tester() && name() == other.name();
    }

    JsonSerializableTester tester() const {
        return JsonSerializableTester(_json["tester"]);
    }

    std::string name() const {
        return _json["name"].as_string();
    }

    const jsoncons::ojson& to_json() const {
        return _json;
    }

private:
    jsoncons::ojson _json;
};

class JsonEncoderTest : public ::testing::Test {
public:
    static const std::string json_str;
    static const std::string holder_json_str;
};

const std::string JsonEncoderTest::json_str = R"({"n":1,"f":2.2,"d":3.33,"str":"test string"})";

const std::string JsonEncoderTest::holder_json_str = R"(
    {
        "tester": {"n": 1, "f": 2.2, "d": 3.33, "str": "test string"},
        "name": "tester holder"
    }
)";


TEST_F(JsonEncoderTest, test_basic_encoding) {
    JsonSerializableTester basic_obj(1, 2.2, 3.33, "test string");
    std::string encoded_json_str = utils::encoding::encode_json(basic_obj);
    EXPECT_EQ(basic_obj, utils::encoding::decode_json<JsonSerializableTester>(encoded_json_str));
    EXPECT_EQ(basic_obj, utils::encoding::decode_json<JsonSerializableTester>(json_str));
}

TEST_F(JsonEncoderTest, test_nested_encoding) {
    JsonSerializableTester basic_obj(1, 2.2, 3.33, "test string");
    JsonSerializableTesterHolder holder(basic_obj, "tester holder");
    std::string encoded_json_str = utils::encoding::encode_json(holder);
    EXPECT_EQ(holder, utils::encoding::decode_json<JsonSerializableTesterHolder>(encoded_json_str));
    EXPECT_EQ(holder, utils::encoding::decode_json<JsonSerializableTesterHolder>(holder_json_str));
}