#include <gtest/gtest.h>
#include <iostream>

#include "utils/encoding/json_encoder.h"


class JsonSerializableTester {
public:

    JsonSerializableTester(int n, float f, double d, const std::string& str) {
        _n = n;
        _f = f;
        _d = d;
        _str = str;
    }

    JsonSerializableTester(): _n(0), _f(0), _d(0), _str("") {}

    bool operator ==(const JsonSerializableTester& other) const {
        return n() == other.n() && f() == other.f() && d() == other.d() && str() == other.str();
    }

    int n() const {
        return _n;
    }

    float f() const {
        return _f;
    }

    double d() const {
        return _d;
    }

    const std::string& str() const {
        return _str;
    }

private:
    int _n;
    float _f;
    double _d;
    std::string _str;
};


class JsonSerializableTesterHolder {
public:

    JsonSerializableTesterHolder(const JsonSerializableTester& tester, const std::string& name):
        _tester(tester), _name(name)
    {
    }

    bool operator ==(const JsonSerializableTesterHolder& other) const {
        return tester() == other.tester() && name() == other.name();
    }

    const JsonSerializableTester& tester() const {
        return _tester;
    }

    const std::string& name() const {
        return _name;
    }

private:
    JsonSerializableTester _tester;
    std::string _name;
};

JSONCONS_ALL_GETTER_CTOR_TRAITS(JsonSerializableTester, n, f, d, str)
JSONCONS_ALL_GETTER_CTOR_TRAITS(JsonSerializableTesterHolder, tester, name)

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