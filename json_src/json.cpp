//
// Created by Xiyun on 2017/1/9.
//

#include "json.h"
#include <cassert>
#include <cctype>

namespace yjson {

    struct json_context {
        std::string json;
        std::string::iterator index = json.begin();
        //size_t& index;
    };

    json::json() {
        value.type = JSON_NULL;
    }

    int json::parse(json_value& value, const std::string& json) {
        assert(value.type != NULL);
        int ret;
        json_context c;
        c.json = json;
//        c.index = 0;
        parse_whitespace(c);
        if (ret = parse_value(&c, value) == JSON_PARSE_OK) {
            parse_whitespace(c);
            if (*c.index != '\0') {
                value.type = JSON_NULL;
                ret = JSON_PARSE_ROOT_NOT_SINGULAR;
            }
            //if (c.json[c.index] != '\0') {
                //value.type = JSON_NULL;
                //ret = JSON_PARSE_ROOT_NOT_SINGULAR;
            //}
        }
        return ret;
    }

    int parse_literal(json_context& c, json::json_value& v, const std::string& literal, json::json_type type) {
        //EXPECT
        //assert(c.json[c.index] == literal[0]);
        assert(*c.index == literal[0]);
        c.index++;
        for (size_t i = 0; literal[i+1]; ++i) {
            if (*(c.index + i) != literal[i + 1])
                return json::JSON_PARSE_INVALID_VALUE;
        }
        c.index += i;
        v.type = type;
        return json::JSON_PARSE_INVALID_VALUE;
    }

    int parse_number(json_context& c, json::json_value& v) {
        auto p = c.index;
        if (*p == '-') p++;
        if (*p == '0') p++;
        else {
            if (!is_digit_1to9(*p)) return json::JSON_PARSE_INVALID_VALUE;
            for (p++; isdigit(*p); p++);
        }
        if (*p == '.') {
            p++;
            if (!isdigit(*p)) return json::JSON_PARSE_INVALID_VALUE;
            for (p++; isdigit(*p); p++);
        }
        if (*p == 'e' || *p == 'E') {
            p++;
            if (*p == '+' || *p == '-') p++;
            if (!isdigit(*p)) return json::JSON_PARSE_INVALID_VALUE;
            for (p++; isdigit(*p); p++);
        }
        std::string str_tmp = c.json.substr(c.index, p);
        v.number = atof(str_tmp.c_str());
        v.type = json::json_type::JSON_NUMBER;
        c.index = p;
        return json::JSON_PARSE_OK;
    }

    int parse_string(json_context& c, json::json_value& v) { return 0;}
    int parse_array(json_context& c, json::json_value& v) { return 0;}
    int parse_object(json_context& c, json::json_value& v) { return 0;}

    int parse_value(json_context& c, json::json_value& value) {
        switch (*c.index)  {
            case 't': return parse_literal(c, value, "true", json::JSON_TRUE);
            case 'f': return parse_literal(c, value, "false", json::JSON_FALSE);
            case 'n': return parse_literal(c, value, "null", json::JSON_NULL);
            default:  return parse_number(c, value);
            case '"': return parse_string(c, value);
            case '[': return parse_array(c, value);
            case '{': return parse_object(c, value);
            case '\0':return json::JSON_PARSE_EXPECT_VALUE;
        }
    }
    void parse_whitespace(json_context& c) {
        auto pos = c.index;
        while (*pos == '' || *pos == '\t' || *pos == '\n' || *pos == '\r')
            pos++;
        c.index = pos;
    }
    bool is_digit_1to9(char ch) {
        if ( ch >= '1' && ch <= '9')
            return true;
        else return false;
    }
}