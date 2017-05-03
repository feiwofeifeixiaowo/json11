//
// Created by Xiyun on 2017/4/22.
//

#include "json.h"
#include <iostream>
#include <stdexcept> // runtime error
#include <cstring> // strncmp
#include <cstdlib> //strtod
#include <cmath> // HUGE_VAL
#include <cassert> // assert
#include <utility>  // move

using namespace std;
using namespace xjson;
using namespace details;


namespace details {

bool is1to9(char ch) { return (ch) >= '1' && (ch) <='9'; }
bool is0to9(char ch) { return (ch) >= '0' && (ch) <= '9'; }
class Parser final {
public:
    Parser(const string& content) : start_(content.c_str()), curr_(content.c_str()) { }
    Json parse() {
        parseWhitespace();
        Json json = parseValue();
        parseWhitespace();
        if(*curr_) error("ROOT NOT SINGULAR");
        return json;
    }

private:
    Json parseValue() {
        switch (*start_){
            case 'n':
                return parseLiteral("null");
            case 't':
                return parseLiteral("true");
            case 'f':
                return parseLiteral("false");
            case '\"':
                return parseString();
            case '[':
                return parseArray();
            case '{':
                return parseObject();
            case '\0':
                error("EXPECT VALUE");
            default:
                return parseNumber();
        }
    }

    /* 使用 parseLiteral合并 true false null的parse
    Json parseNull() {
        if(strncmp(curr_, "null", 4)) error("INVALID VALUE");
        curr_ += 4;
        start_ = curr_;
        return Json(nullptr);
    }

    Json parseTrue() {
        if(strncmp(curr_, "true", 4)) error("INVALID VALUE");
        curr_ +=4;
        start_ = curr_;
        return Json(true);
    }

    Json parseFalse() {
        if(strncmp(curr_, "false", 5)) error("INVALID VALUE");
        curr_ +=5;
        start_ = curr_;
        return Json(false);
    }
     */

    Json parseLiteral(const string& literal) {
        if(strncmp(curr_, literal.c_str(), literal.size()))error("INVALID VALUE");
        curr_ += literal.size();
        start_ = curr_;
        switch (literal[0]) {
            case 't':
                return Json(true);
            case 'f':
                return Json(false);
            default:
                return Json(nullptr);
        }
    }

    void parseWhitespace() noexcept {
        while (*curr_ == ' ' || *curr_ == '\t' || *curr_ == '\n' || *curr_ == '\r')
            curr_++;
        start_ = curr_;
    }

    Json parseNumber() {
        if(*curr_ == '-') ++curr_;
        if(*curr_ == '0') ++curr_;
        else {
            if(!is1to9(*curr_)) error("INVALID VALUE");
            for(++curr_; is0to9(*curr_); ++curr_);
//            while(is0to9(*++curr_))
//                ;
        }
        // frac
        if(*curr_ == '.') {
            if(!is0to9(*++curr_)) error("INVALID VALUE");
            for(++curr_; is0to9(*curr_); ++curr_);
//            while(is0to9(*++curr_))
//                ;
        }
        // exp
        if(*curr_ == 'e' || *curr_ == 'E') {
            ++curr_;
            if(*curr_ == '+' || *curr_ == '-') ++curr_;
            if(!is0to9(*curr_)) error("INVALID VALUE");
            for(++curr_; is0to9(*curr_); ++curr_);
//            while(is0to9(*++curr_))
//                ;
        }

        double val = strtod(start_, nullptr);
        if(fabs(val) == HUGE_VAL) error("NUMBER TOO BIG");
        start_ = curr_;
        return Json(val);
    }

    Json parseString() { return Json(parseRawString()); }

    string parseRawString() {
        string str;
        for(;;) {
            switch (*++curr_) {
                case '\"':
                    start_ = ++curr_;
                    return str;  // 空字符串
                case '\0':
                    error("MISS QUOTATION MARK");
                default:
                    if(static_cast<unsigned char>(*curr_) < 0x20)
                        error("INVALID STRING CHAR");
                    str.push_back(*curr_);
                    break;
                case '\\':
                    switch (*++curr_) {
                        case '\"':
                            str.push_back('\"');
                            break;
                        case '\\':
                            str.push_back('\\');
                            break;
                        case '/':
                            str.push_back('/');
                            break;
                        case 'b':
                            str.push_back('\b');
                            break;
                        case 'f':
                            str.push_back('\f');
                            break;
                        case 'n':
                            str.push_back('\n');
                            break;
                        case 'r':
                            str.push_back('\r');
                            break;
                        case 't':
                            str.push_back('\t');
                            break;
                        case 'u': {
                            unsigned u1 = parseHex4();
                            if(u1 >= 0xD800 && u1 <= 0xDBFF) {
                                if(*++curr_ != '\\') error("INVALID UNICODE SURROGATE");
                                if(*++curr_ != 'u' ) error("INVALID UNICODE SURROGATE");
                                unsigned u2 = parseHex4();
                                if(u2 < 0xDC00 || u2 > 0xDFFF) error("INVALID UNICODE SURROGATE");
                                u1 = (((u1 - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
                            }
                            str += encodeUTF8(u1);
                        }
                            break;
                        default:
                            error("INVALID STRING ESCAPE");
                    }
                    break;  // break from case '\\'
            }
        }  // for loop
    }

    unsigned parseHex4() {
        unsigned u = 0;
        for(int i = 0; i<4; ++i) {
            unsigned ch = static_cast<unsigned>(toupper(*++curr_));
            u <<= 4;
            if      (ch >= '0' && ch <='9')  u |= ch - '0';
            else if (ch >= 'A' && ch <= 'F') u |= ch - ('A' - 10);
            else error("INVALID UNICODE HEX");
        }
        return u;
    }

    string encodeUTF8(unsigned u) {
       string utf8;
        if(u < 0x7F)
            utf8.push_back(static_cast<char>(u & 0xFF));
        else if(u <= 0xFF) {
            utf8.push_back(static_cast<char>(0xC0 | ((u >> 6) & 0xFF)));
            utf8.push_back(static_cast<char>(0x80 | ( u       & 0x3F)));
        }
        else if(u <= 0xFFFF) {
            utf8.push_back(static_cast<char>(0xE0 | ((u >> 12) & 0xFF)));
            utf8.push_back(static_cast<char>(0x80 | ((u >> 6)  & 0x3F)));
            utf8.push_back(static_cast<char>(0x80 | ( u        & 0x3F)));
        }
        else {
            assert(u <= 0x10FFFF);
            utf8.push_back(static_cast<char>(0xF0 | ((u >> 18) & 0xFF)));
            utf8.push_back(static_cast<char>(0x80 | ((u >> 12) & 0x3F)));
            utf8.push_back(static_cast<char>(0x80 | ((u >> 6)  & 0x3F)));
            utf8.push_back(static_cast<char>(0x80 | ( u        & 0x3F)));
        }
        return utf8;
    }

    Json parseArray() {
        Json::array_t arr;
        ++curr_;
        parseWhitespace();
        if(*curr_ == ']') {
            start_ = ++curr_;
            return Json(arr);
        }
        for(;;) {
            parseWhitespace();
            arr.push_back(parseValue());
            parseWhitespace();
            if(*curr_ == ',') ++curr_;
            else if(*curr_ == ']') {
                start_ = ++curr_;
                return Json(arr);
            }
            else error("MISS COMMA OR SQUARE BRACKET");
        }
    }

    Json parseObject() {
        Json::object_t obj;
        ++curr_;
        parseWhitespace();
        if(*curr_ == '}') {
            start_ = ++curr_;
            return Json(obj);
        }
        for(;;) {
            parseWhitespace();
            if (*curr_ != '"') error("MISS KEY");
            string key = parseRawString();
            parseWhitespace();
            if (*curr_++ != ':') error("MISS COLON");
            parseWhitespace();
            Json val = parseValue();
            obj.insert({key, val});
            parseWhitespace();
            if (*curr_ == ',') ++curr_;
            else if (*curr_ == '}') {
                start_ = ++curr_;
                return Json(obj);
            }else error("MISS COMMA OR CURLY BRACKET");
        }
    }
    // utils func
    [[noreturn]] void error(const string& msg) const {
        throw runtime_error(msg + ": " + start_);
    }

private:
    const char* start_;
    const char* curr_;
}; // class Parser
} // namespace details

namespace xjson {
    Json::JsonType Json::type() const { return value_->type(); }
    Json::Json(std::nullptr_t) : value_(make_unique<JsonNull>()){ }
    Json::Json(bool val) :value_(make_unique<JsonBool>(val)){ }
    Json::Json(double val) :value_(make_unique<JsonNumber>(val)){ }
    Json::Json(const std::string &val) :value_(make_unique<JsonString>(val)) { }
    Json::Json(const char *val) :value_(make_unique<JsonString>(val)) { }
    Json::Json(const array_t &val) :value_(make_unique<JsonArray>(val)) { }
    Json::Json(const object_t &val) :value_(make_unique<JsonObject>(val)) { }

    Json::Json(const Json& rhs) {
        switch (rhs.type()) {
            case JsonType::kNull:
            value_ = make_unique<JsonNull>();
                break;
            case JsonType::kBool:
                value_ = make_unique<JsonBool>(rhs.asBool());
                break;
            case JsonType::kNumber:
                value_ = make_unique<JsonNumber>(rhs.asDouble());
                break;
            case JsonType::kString:
                value_ = make_unique<JsonString>(rhs.asString());
                break;
            case JsonType::kArray:
                value_ = make_unique<JsonArray>(rhs.asArray());
                break;
            case JsonType::kObject:
                value_  = make_unique<JsonObject>(rhs.asObject());
                break;
        }
    }

    Json::Json(Json &&rhs) noexcept :value_(std::move(rhs.value_)){ }
    Json::~Json() { }

    void Json::swap(Json &rhs) noexcept {
        using std::swap;
        swap(value_, rhs.value_);
    }

    Json& Json::operator=(Json rhs) {
        swap(rhs);
        return *this;
    }

    bool Json::operator==(const Json &rhs) const {
        if(value_.get() == rhs.value_.get()) return true;
        if(type() != rhs.type()) return false;
        switch (type()) {
            case JsonType::kNull:
                return true;
            case JsonType::kBool:
                return asBool() == rhs.asBool();
            case JsonType::kNumber:
                return asDouble() == rhs.asDouble();
            case JsonType::kString:
                return asString() == rhs.asString();
            case JsonType::kArray:
                return asArray() == rhs.asArray();
            case JsonType::kObject:
                return asObject() == rhs.asObject();
        }
//        assert(0);  //
    }

    bool Json::operator!=(const Json &rhs) const { return !(*this == rhs); }
    // compare
    bool Json::isNull() const { return type() == JsonType::kNull; }
    bool Json::isBool() const { return type() == JsonType ::kBool; }
    bool Json::isNumber() const { return type() == JsonType ::kNumber; }
    bool Json::isString() const { return type() == JsonType::kString; }
    bool Json::isArray() const { return type() == JsonType::kArray; }
    bool Json::isObject() const { return type() == JsonType::kObject; }

    bool Json::asBool() const { return value_->asBool(); }
    double Json::asDouble() const {return value_->asDouble(); }
    const std::string& Json::asString() const { return value_->asString(); }
    Json::array_t& Json::asArray() { return value_->asArray(); }
    const Json::array_t& Json::asArray() const { return value_->asArray(); }

    Json::object_t& Json::asObject() { return value_->asObject(); }
    const Json::object_t& Json::asObject() const { return value_->asObject(); }

    Json& Json::operator[](std::size_t i) { return value_->operator[](i); }
    const Json& Json::operator[](std::size_t i) const { return value_->operator[](i); }

    Json& Json::operator[](const std::string& i) { return value_->operator[](i); }
    const Json& Json::operator[](const std::string &i) const { return value_->operator[](i); }

    std::size_t Json::size() const { return value_->size(); }
    std::string Json::serialize() const {
        switch (value_->type()) {
            case JsonType::kNull:
                return "null";
            case JsonType::kBool:
                return value_->asBool() ? "true" : "false";
            case JsonType::kNumber:
                char buf[32];
                sprintf(buf, "%.17g", value_->asDouble());
                return buf;
            case JsonType::kString:
                return serializeString();
            case JsonType::kArray:
                return serializeArray();
            case JsonType::kObject:
                return serializeObject();
        }
    }

    std::string Json::serializeString() const {
        string ret = "\"";
        for(auto e : value_->asString()) {
            switch (e) {
                case '\"':
                    ret += "\\\"";
                    break;
                case '\\':
                    ret += "\\\\";
                    break;
                case '\b':
                    ret += "\\b";
                    break;
                case '\f':
                    ret += "\\f";
                    break;
                case '\n':
                    ret += "\\n";
                    break;
                case '\r':
                    ret += "\\r";
                    break;
                case '\t':
                    ret += "\\t";
                    break;
                default:
                    if(static_cast<unsigned  char>(e) < 0x20) {
                        char buf[7];
                        sprintf(buf, "\\u%04X", e);
                        ret += buf;
                    }else
                        ret += e;
            }
        }
        return ret + '"';
    }

    std::string Json::serializeArray() const {
        string ret = "[ ";
        for(size_t i = 0; i < value_->size(); ++i) {
            if(i > 0) ret += ", ";
            ret += (*this)[i].serialize();
        }
        return ret += " ]";
    }

    std::string Json::serializeObject() const {
        string ret = "{ ";
        bool first = true;
        for(const pair<string, Json>& p : value_->asObject()) {
            if(first)
                first = false;
            else
                ret += ", ";
            ret += "\"" + p.first + "\"";
            ret += ": ";
            ret += p.second.serialize();
        }
        return ret + " }";
    }
    // parse func
    Json Json::parse(const std::string &content, std::string &errMsg) {
        try {
            Parser p(content);
            return p.parse();
        } catch (runtime_error& e){
            errMsg = e.what();
            return Json(nullptr);
        }
    }

}