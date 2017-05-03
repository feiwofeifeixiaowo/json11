//
// Created by Xiyun on 2017/4/22.
//


#ifndef JSONCPP_JSON_H
#define JSONCPP_JSON_H

#include <cstddef>
#include <string>
#include <stdexcept>  // runtime_error
#include <vector>  // json array
#include <unordered_map>  // json object

namespace details {
class JsonValue;
}

namespace xjson {
class Json final {
public:
    using array_t = std::vector<Json>;
    using object_t = std::unordered_map<std::string, Json>;

    // Json 中的6种数据类型
    enum class JsonType {
        // Null Value
        kNull = 1,
        // Boolean Value
        kBool = 2,
        // Number Value
        kNumber = 3,
        // String Value
        kString = 4,
        // Array Value
        kArray = 5,
        // Obejct Value
        kObject = 6
    };

    // Constructor
    explicit Json(std::nullptr_t);
    explicit Json(bool);
    explicit Json(double);
    explicit Json(const std::string&);
    explicit Json(const char*);
    explicit Json(const array_t&);
    explicit Json(const object_t&);

    ~Json();
    // copy constructor
    Json(const Json&);
    // move constructor
    Json(Json&&) noexcept;
    // copy operator
    Json& operator=(Json);
    // swap two json values
    void swap(Json&) noexcept;

    // compare
    bool operator==(const Json&) const;
    bool operator!=(const Json&) const;

    // parser func
    static Json parse(const std::string& content, std::string& errMsg);

// get the type of Json value
    JsonType type() const;
    bool isNull() const;
    bool isBool() const;
    bool isNumber() const;
    bool isString() const;
    bool isArray() const;
    bool isObject() const;

    bool asBool() const;
    double asDouble() const;
    const std::string& asString() const;
    array_t& asArray();
    const array_t& asArray() const;

    object_t& asObject();
    const object_t& asObject() const;

    // Accesses a field of json array
    Json& operator[](std::size_t);
    const Json& operator[](std::size_t) const;

    Json& operator[](const std::string&);
    const Json& operator[](const std::string&) const;

    std::size_t size() const;
    std::string serialize() const;

private:
    std::string serializeString() const;
    std::string serializeArray() const;
    std::string serializeObject() const;
    std::shared_ptr<details::JsonValue> value_;

};  // class Json

inline std::ostream& operator<<(std::ostream& os, const Json& json) {
    return os << json.serialize();
}
}  // namespace xjson

namespace details {
class JsonValue {
public:
    virtual xjson::Json::JsonType type() const = 0;
    virtual size_t size() const { return 0; }
    virtual bool asBool() const { throw std::runtime_error("not a boolean"); }
    virtual double asDouble() const { throw std::runtime_error("not a number"); }
    virtual const std::string& asString() const { throw std::runtime_error("not a string"); }
    virtual xjson::Json::array_t& asArray() { throw std::runtime_error("not an array"); }
    virtual const xjson::Json::array_t& asArray() const { throw std::runtime_error("not an array"); }

    virtual xjson::Json::object_t& asObject() { throw std::runtime_error("not an object"); }
    virtual const xjson::Json::object_t& asObject() const { throw std::runtime_error("not an object"); }

    virtual xjson::Json& operator[](size_t) { throw std::runtime_error("not an array"); }
    virtual const xjson::Json& operator[](size_t) const { throw std::runtime_error("not an array"); }

    virtual xjson::Json& operator[](const std::string&) { throw std::runtime_error("not an object"); }
    virtual const xjson::Json& operator[](const std::string&) const { throw std::runtime_error("not an object"); }
    virtual ~JsonValue() { }
    JsonValue() { }

};

class JsonNull final : public JsonValue {
public:
    explicit JsonNull() { }
    xjson::Json::JsonType type() const override { return xjson::Json::JsonType::kNull; }
};

class JsonBool final : public JsonValue {
public:
    explicit JsonBool(bool val) :val_(val) { }
    xjson::Json::JsonType type() const override { return xjson::Json::JsonType::kBool; }
    bool asBool() const override { return val_; }
private:
    bool val_;
};

class JsonNumber final : public JsonValue {
public:
    explicit JsonNumber(double val) :val_(val) { }
    xjson::Json::JsonType  type() const override { return xjson::Json::JsonType::kNumber; }
    double asDouble() const override {return val_;}
private:
    double val_;
};

class JsonString final : public JsonValue {
public:
    explicit JsonString(const std::string& val) :val_(val) { }
    xjson::Json::JsonType type() const override { return xjson::Json::JsonType::kString; }
    const std::string& asString() const override {return val_; }
private:
    std::string val_;
};

class JsonArray final : public JsonValue {
public:
    explicit JsonArray(const xjson::Json::array_t& val) : val_(val) { };
    xjson::Json::JsonType type() const  override { return xjson::Json::JsonType::kArray; }
    xjson::Json::array_t& asArray() override { return val_; }
    const xjson::Json::array_t& asArray() const override { return val_; }
    const xjson::Json& operator[](size_t i) const override { return val_[i]; }
    xjson::Json& operator[](size_t i) override { return val_[i]; }
    size_t size() const override { return val_.size(); }
private:
    xjson::Json::array_t val_;
};

class JsonObject final : public JsonValue {
public:
    explicit JsonObject(const xjson::Json::object_t& val) : val_(val) { };
    xjson::Json::JsonType type() const  override { return xjson::Json::JsonType::kObject; }
    xjson::Json::object_t& asObject() override { return val_; }
    const xjson::Json::object_t& asObject() const override { return val_; }
    const xjson::Json& operator[](const std::string& i) const override { return val_.at(i); }
    xjson::Json& operator[](const std::string& i) override { return val_.at(i); }
    size_t size() const override { return val_.size(); }
private:
    xjson::Json::object_t val_;
};
}  // details

#endif //JSONCPP_JSON_H
