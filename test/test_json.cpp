//
// Created by Xiyun on 2017/4/22.
//

#include "gtest/gtest.h"
#include "../json.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using std::string;
using namespace std;
using namespace xjson;

//TDD test driven dev
Json parseOK(const string& json) {
    string errMsg;
    Json ret = Json::parse(json, errMsg);
    EXPECT_EQ(errMsg, "");
    return ret;
}
void testNumber(double expect, const string& json) {
    Json ret = parseOK(json);
    EXPECT_TRUE(ret.isNumber());
    EXPECT_EQ(ret.asDouble(), expect);
}

void testNull(const string& json) {
    Json ret = parseOK(json);
    EXPECT_TRUE(ret.isNull());
}

void testBool(bool expect, const string& json) {
    Json ret = parseOK(json);
    EXPECT_TRUE(ret.isBool());
    EXPECT_EQ(ret.asBool(), expect);
}

void testError(const string& expect, const string& json) {
    string errMsg;
    Json ret = Json::parse(json, errMsg);
    auto pos = errMsg.find_first_of(":");
    auto actual = errMsg.substr(0, pos);
    EXPECT_EQ(actual, expect);
}

void testString(const string& expect, const string& json) {
    Json ret = parseOK(json);
    EXPECT_TRUE(ret.isString());
//    cout << endl << "TEST: " << ret.asString().size() << endl;
    EXPECT_EQ(ret.asString(), expect);
}

TEST(Str2Json, JsonNull) {
//    string json = "null";
//    string json = "  null\n\r\t";
    testNull("null");
    testNull("\t\r\n null\t\r\n");
}

TEST(Str2Json, JsonBool) {
    testBool(true, "true");
    testBool(false, "false");
}

/* 改用 testBool()
TEST(Str2Json, JsonFalse) {
    string json = "\t\rfalse";
    string errMsg;
    Json ret = Json::parse(json, errMsg);
    EXPECT_EQ(errMsg, "");
    EXPECT_EQ(ret.asBool(), false);
}
 */

TEST(Str2Json, JsonNumber) {
    testNumber(0.0, "0");
    testNumber(0.0, "-0");
    testNumber(0.0, "-0.0");
    testNumber(1.0, "1");
    testNumber(-1.0, "-1");
    testNumber(1.5, "1.5");
    testNumber(-1.5, "-1.5");
    testNumber(3.1416, "3.1416");
    testNumber(1E10, "1E10");
    testNumber(1e10, "1e10");
    testNumber(1E-10, "1E-10");
    testNumber(1E+10, "1E+10");
    testNumber(-1E10, "-1E10");
    testNumber(-1e10, "-1e10");
    testNumber(-1E+10, "-1E+10");
    testNumber(-1E-10, "-1E-10");
    testNumber(1.234E+10, "1.234E+10");
    testNumber(1.234E-10, "1.234E-10");
    testNumber(0.0, "1e-10000");  /* must underflow */

    testNumber(1.0000000000000002, "1.0000000000000002");
    testNumber(4.9406564584124654e-324, "4.9406564584124654e-324");
    testNumber(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    testNumber(2.2250738585072009e-308, "2.2250738585072009e-308");
    testNumber(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    testNumber(1.7976931348623157e+308, "1.7976931348623157e+308");
    testNumber(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

TEST(Str2Json, JsonString) {
    testString("", "\"\"");
    testString("Hello", "\"Hello\"");
    testString("Hello\nWorld", "\"Hello\\nWorld\"");
    testString("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");

    string s = "Hello";
    s.push_back('\0');
    s += "World";
    testString(s, "\"Hello\\u0000World\"");
    testString("\x24", "\"\\u0024\"");
    testString("\xC2\xA2", "\"\\u00A2\"");
    testString("\xE2\x82\xAC", "\"\\u20AC\"");
    testString("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");
    testString("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");
    testString("\xE0\x84\xA3\xE4\x95\xA7\xE8\xA6\xAB\xEC\xB7\xAF\xEA\xAF\x8D\xEE\xBD\x8A", "\"\\u0123\\u4567\\u89AB\\uCDEF\\uabcd\\uef4A\"");


    string errMsg;
    Json ret = Json::parse("\"hello\"", errMsg);
    ret = Json("again");
    EXPECT_EQ(ret.asString(), "again");
}

TEST(Str2Json, JsonArray) {
    Json ret1 = parseOK("[ ]");
    EXPECT_TRUE(ret1.isArray());
    EXPECT_EQ(ret1.size(), 0);

    Json ret2 = parseOK("[ null, false, true, 123, \"abc\" ]");
    EXPECT_TRUE(ret2.isArray());
    EXPECT_EQ(ret2.size(), 5);
    EXPECT_EQ(ret2[0], Json(nullptr));
    EXPECT_EQ(ret2[1], Json(false));
    EXPECT_EQ(ret2[2], Json(true));
    EXPECT_EQ(ret2[3], Json(123.0));
    EXPECT_EQ(ret2[4], Json("abc"));
}

TEST(Str2Json, JsonObject) {
    Json ret1 = parseOK("{ }");
    EXPECT_TRUE(ret1.isObject());
    EXPECT_EQ(ret1.size(), 0);

    string errMsg;
    Json ret2 = Json::parse(
            " { "
            "\"n\" : null , "
            "\"f\" : false , "
            "\"t\" : true , "
            "\"i\" : 123 , "
            "\"s\" : \"abc\", "
            "\"a\" : [1, 2, 3 ], "
            "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" :  3 }"
            " } ", errMsg);
    EXPECT_TRUE(ret2.isObject());
    EXPECT_EQ(ret2.size(), 7);

    EXPECT_TRUE(ret2["n"].isNull());
    EXPECT_TRUE(ret2["f"].isBool());
    EXPECT_EQ(ret2["f"].asBool(), false);

    EXPECT_TRUE(ret2["t"].isBool());
    EXPECT_EQ(ret2["t"].asBool(), true);

    EXPECT_TRUE(ret2["i"].isNumber());
    EXPECT_EQ(ret2["i"].asDouble(), 123.0);

    EXPECT_TRUE(ret2["s"].isString());
    EXPECT_EQ(ret2["s"].asString(), "abc");

    EXPECT_TRUE(ret2["a"].isArray());
    EXPECT_EQ(ret2["a"].size(), 3);

    EXPECT_TRUE(ret2["o"].isObject());
    EXPECT_EQ(ret2["o"].size(), 3);

}

TEST(Error, ExpectValue) {
    testError("EXPECT VALUE", "");
    testError("EXPECT VALUE", "  ");
}

TEST(ERROR, InvalidValue) {
    testError("INVALID VALUE", "nul");
    testError("INVALID VALUE", "?");
    testError("INVALID VALUE", "+0");
    testError("INVALID VALUE", "+1");
    testError("INVALID VALUE", ".123");
    testError("INVALID VALUE", "1.");
    testError("INVALID VALUE", "INF");
    testError("INVALID VALUE", "inf");
    testError("INVALID VALUE", "NAN");
    testError("INVALID VALUE", "nan");
    testError("INVALID VALUE", "[1,]");
    testError("INVALID VALUE", "[\"a\", nul]");
}

TEST(ERROR, RootNotSingular) {
    testError("ROOT NOT SINGULAR", "null x");
    testError("ROOT NOT SINGULAR", "0123");
    testError("ROOT NOT SINGULAR", "0x0");
    testError("ROOT NOT SINGULAR", "0x123");
}

TEST(ERROR, NumberTooBig) {
#if 1
    testError("NUMBER TOO BIG", "1e309");
    testError("NUMBER TOO BIG", "-1e309");
#endif
}

TEST(ERROR, MissQuotaMark) {
    testError("MISS QUOTATION MARK", "\"");
    testError("MISS QUOTATION MARK", "\"abc");
}

TEST(ERROR, InvalidStringEscape) {
    testError("INVALID STRING ESCAPE", "\"\\v\"");
    testError("INVALID STRING ESCAPE", "\"\\'\"");
    testError("INVALID STRING ESCAPE", "\"\\0\"");
    testError("INVALID STRING ESCAPE", "\"\\x12\"");
}

TEST(ERROR, InvalidStringChar) {
    testError("INVALID STRING CHAR", "\"\x01\"");
    testError("INVALID STRING CHAR", "\"\x1F\"");
}

TEST(ERROR, InvalidUnicodeHex) {
    testError("INVALID UNICODE HEX", "\"\\u\"");
    testError("INVALID UNICODE HEX", "\"\\u0\"");
    testError("INVALID UNICODE HEX", "\"\\u01\"");
    testError("INVALID UNICODE HEX", "\"\\u012\"");
    testError("INVALID UNICODE HEX", "\"\\u/000\"");
    testError("INVALID UNICODE HEX", "\"\\uG000\"");
    testError("INVALID UNICODE HEX", "\"\\u0/00\"");
    testError("INVALID UNICODE HEX", "\"\\u0G00\"");
    testError("INVALID UNICODE HEX", "\"\\u000/\"");
    testError("INVALID UNICODE HEX", "\"\\u00G/\"");
    testError("INVALID UNICODE HEX", "\"\\u 123/\"");
}

TEST(ERROR, InvalidUnicodeSurrogate) {
    testError("INVALID UNICODE SURROGATE", "\"\\uD800\"");
    testError("INVALID UNICODE SURROGATE", "\"\\uDBFF\"");
    testError("INVALID UNICODE SURROGATE", "\"\\uD800\\\\\\");
    testError("INVALID UNICODE SURROGATE", "\"\\uD800\\uDBFF\"");
    testError("INVALID UNICODE SURROGATE", "\"\\uD800\\uE000\"");
}

TEST(ERROR, MissCommaOrSquareBracket) {
    testError("MISS COMMA OR SQUARE BRACKET", "[1");
    testError("MISS COMMA OR SQUARE BRACKET", "[1}");
    testError("MISS COMMA OR SQUARE BRACKET", "[1 2");
    testError("MISS COMMA OR SQUARE BRACKET", "[[ ]");
}

TEST(ERROR, MissKey) {
    testError("MISS KEY", "{: 1, }");
    testError("MISS KEY", "{1: 1,");
    testError("MISS KEY", "{true:1,");
    testError("MISS KEY", "{false:1,");
    testError("MISS KEY", "{null:1,");
    testError("MISS KEY", "{[]:1,");
    testError("MISS KEY", "{{}:1,");
    testError("MISS KEY", "{\"a\":1,");
}

TEST(ERROR, MissColon) {
    testError("MISS COLON", "{\"a\"}");
    testError("MISS COLON", "{\"a\", \"b\"}");
}

TEST(ERROR, MissCommaOrCurlyBracket) {
    testError("MISS COMMA OR CURLY BRACKET", "{\"a\":1");
    testError("MISS COMMA OR CURLY BRACKET", "{\"a\":1]");
    testError("MISS COMMA OR CURLY BRACKET", "{\"a\":1 \"b\"");
    testError("MISS COMMA OR CURLY BRACKET", "{\"a\":{}");
}

TEST(Json, Ctor) {
    {
        Json ret(nullptr);
        EXPECT_TRUE(ret.isNull());
    }
    {
        Json ret1(true);
        EXPECT_TRUE(ret1.isBool());
        EXPECT_EQ(ret1.asBool(), true);

        Json ret2(false);
        EXPECT_TRUE(ret2.isBool());
        EXPECT_EQ(ret2.asBool(), false);
    }
    {
        Json ret1(0.0);
        EXPECT_TRUE(ret1.isNumber());
        EXPECT_EQ(ret1.asDouble(), 0.0);

        Json ret2(100.1);
        EXPECT_TRUE(ret2.isNumber());
        EXPECT_EQ(ret2.asDouble(), 100.1);
    }
    {
        Json ret("test");
        EXPECT_TRUE(ret.isString());
        EXPECT_EQ(ret.asString(), "test");
    }
    {
        vector<Json> arr{Json(nullptr), Json(true), Json(1.2)};
        Json ret(arr);
        EXPECT_TRUE(ret.isArray());
        EXPECT_TRUE(ret[1].isBool());
    }
    {
        unordered_map<string, Json> obj;
        obj.insert({"hello", Json(nullptr)});
        obj.insert({"test", Json(true)});
        obj.insert({"world", Json("!!")});

        Json ret(obj);
        EXPECT_TRUE(ret.isObject());
        EXPECT_TRUE(ret["world"].isString());
        EXPECT_TRUE(ret["test"].isBool());
    }
}


void testRoundtrip(const string& expect) {
    Json ret = parseOK(expect);
    string actual = ret.serialize();
    if(ret.isNumber())
        EXPECT_EQ(strtod(actual.c_str(), nullptr), strtod(expect.c_str(), nullptr));
    else
        EXPECT_EQ(actual, expect);
}

TEST(Json2Str, literal) {
    testRoundtrip("0");
    testRoundtrip("true");
    testRoundtrip("null");
    testRoundtrip("false");
}

TEST(Json2Str, JsonNumber) {
    testRoundtrip("0");
    testRoundtrip("-0");
    testRoundtrip("-1");
    testRoundtrip("1");
    testRoundtrip("1.5");
    testRoundtrip("-1.5");
    testRoundtrip("123.3");
    testRoundtrip("1e+20");
    testRoundtrip("1.123e+20");
    testRoundtrip("1.123e-20");

    testRoundtrip("1.0000000000000002");
    testRoundtrip("4.9406564584124654e-324");
    testRoundtrip("-4.9406564584124654e-324");
    testRoundtrip("2.2250738585072009e-308");
    testRoundtrip("-2.2250738585072009e-308");
    testRoundtrip("1.7976931348623157e+308");
    testRoundtrip("-1.7976931348623157e+308");
}

TEST(Json2Str, JsonString) {
    testRoundtrip("\"\"");
    testRoundtrip("\"hello\"");
    testRoundtrip("\"hello\\nworld\"");
    testRoundtrip("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
    testRoundtrip("\"hello\\u0000world\"");
}

TEST(Json2Str, JsonArray) {
    testRoundtrip("[  ]");
    testRoundtrip("[ null, false, true, 123, \"abc\", [ 1, 2, 3, 4 ] ]");
}

TEST(Json2Str, JsonObject) {
    testRoundtrip("{  }");

//    testRoundtrip(R"({ "o": { "3": 3, "2": 2, "1": 1 }, "a": [ 1, 2, 3 ], "s": "abc", "n": null, "f": false, "t": true, "i": 123 })");
}


/*
TEST(IndependentMethod, ResetToZero) {
    int i = 3;
    independentMethod(i);
    EXPECT_EQ(0, i);

    i = 12;
    independentMethod(i);
    EXPECT_EQ(0, i);
}

TEST(IndependentMethod, ResetToZero2) {
    int i = 0;
    independentMethod(i);
    EXPECT_EQ(0, i);
}

class JsonTest : public ::testing::Test {
protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    JsonTest() {
        // You can do set-up work for each test here.
    }

    virtual ~JsonTest() {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:
    virtual void SetUp() {
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    virtual void TearDown() {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    // Objects declared here can be used by all tests in the test case for JsonTest.
    json j;
};

// Test case must be called the class above
// Also note: use TEST_F instead of TEST to access the test fixture (from google test primer)
TEST_F(JsonTest, MethodBarDoesAbc) {
    int i = 0;
    j.foo(i);  // we have access to j, declared in the fixture
    EXPECT_EQ(1,i);
}
 */