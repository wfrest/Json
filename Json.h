#ifndef WFREST_JSON_H_
#define WFREST_JSON_H_

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <functional>
#include <map>
#include <cassert>
#include "json_parser.h"
#include "JsonValue.h"

namespace wfrest
{

class Json
{
public:
    using Object = std::map<std::string, Json>;
public: 
    // Constructors for the various types of JSON value.
    Json() : val_(nullptr) {}
    Json(const std::string& str) : val_(str) {}
    Json(const char* str) : val_(str) {}
    Json(std::nullptr_t null) : val_(null) {}
    Json(double value) : val_(value) {}
    Json(int value) : val_(value) {}
    Json(bool value) : val_(value) {}
    ~Json() = default;

    Json(const Json& json) = delete;
    Json& operator=(const Json& json) = delete;
    Json(Json&& other);
    Json& operator=(Json&& other);

    static Json parse(const std::string &str);
    static Json parse(const std::ifstream& stream);
    
    const std::string dump() const;
    const std::string dump(int spaces) const;

public:
    int type() const
    {
        return val_.type();
    }

    bool is_null() const
    {
        return type() == JSON_VALUE_NULL;
    }

    bool is_number() const
    {
        return type() == JSON_VALUE_NUMBER;
    }

    bool is_boolean() const
    {
        int type = this->type();
        return type == JSON_VALUE_TRUE || type == JSON_VALUE_FALSE;
    }

    bool is_object() const
    {
        return type() == JSON_VALUE_OBJECT;
    }

    bool is_array() const
    {
        return type() == JSON_VALUE_ARRAY;
    }

    bool is_string() const
    {
        return type() == JSON_VALUE_STRING;
    }

    int size() const;

    bool empty() const
    {
        return val_.empty();
    }

    void clear()
    {
        val_.to_object();
    }

public:
    // object
    Json& operator[](const std::string& key);
    
    template <typename T>
    Json& operator=(const T& val)
    {
        Json* json = this->parent_;
        assert(json->type() == JSON_VALUE_OBJECT);
        json->push_back(json->key_, val);
        return *this;
    }

    template <typename T>
    void push_back(const std::string& key, const T& val)
    {
        val_.push_back(key, val);
    }

    template <typename T>
    void push_back(const T& val)
    {
        val_.push_back(val);
    }
    
private:    
    friend inline std::ostream& operator << (std::ostream& os, const Json& json) { return (os << json.dump()); }

private:
    Object object_;
    std::string key_;
    Json* parent_ = nullptr;  // watcher
    JsonValue val_;
};

}  // namespace wfrest

#endif  // WFREST_JSON_H_
