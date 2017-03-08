//
// Created by Xiyun on 2017/1/9.
//

#ifndef JSON11_JSON_H
#define JSON11_JSON_H

#include <map>
#include <vector>
#include <string>

namespace yjson {

    class json {
    public:
        typedef enum {
            JSON_NULL, JSON_FALSE, JSON_TRUE, JSON_NUMBER, JS0N_STRING, JSON_ARRAY, JSON_OBJECT
        } json_type;

        enum {
            JSON_PARSE_OK = 0,
            JSON_PARSE_EXPECT_VALUE,
            JSON_PARSE_INVALID_VALUE,
            JSON_PARSE_ROOT_NOT_SINGULAR,
            JSON_PARSE_NUMBER_TOO_BIG,
            JSON_PARSE_MISS_QUOTATION_MARK,
            JSON_PARSE_INVALID_STRING_ESCAPE,
            JSON_PARSE_INVALID_STRING_CHAR,
            JSON_PARSE_INVALID_UNICODE_HEX,
            JSON_PARSE_INVALID_UNICODE_SURROGATE,
            JSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
            JSON_PARSE_MISS_KEY,
            JSON_PARSE_MISS_COLON,
            JSON_PARSE_MISS_COMMA_OR_CURLY_BARCKET
        };
    private:
        struct json_value;  // front declare

        struct json_value {
            union {
                std::map<std::string, json_value> obj;   // object
                std::vector<json_value> arr;             // array
                std::string str;                         // string
                double number;                           // number
            };
            json_type type;
        };

    public:
        json();

        ~json();

        int parse(json_value &value, const std::string &json);

        std::string &stringify(const json_value &value, size_t length);

        json_type get_type(const json_value &value);

        bool get_boolean(const json_value &value);

        void set_boolean(json_value &value, bool b);

        double get_number(const json_value &value);

        void set_number(json_value &value, double number);

        std::string &get_string(const json_value &value);

        void set_string(json_value &value, const std::string &str);

        json_value &get_array_element(const json_value &value, size_t index);

        json_value &get_object_element(const json_value &value, size_t index);

    public:
        json_value value;
    };
}// namespace json
#endif //JSON11_JSON_H
