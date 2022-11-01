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

namespace wfrest
{

class Json
{
public:
    struct Object
    {
        Object() = default;
        ~Object() = default;
    };

    struct Array
    {
        Array() = default;
        ~Array() = default;
    };

public: 
    // Constructors for the various types of JSON value.
    Json();
    Json(const std::string& str);
    Json(const char* str);
    Json(std::nullptr_t null);
    Json(double val);
    Json(int val);
    Json(bool val);
    Json(const Array& val);
    Json(const Object& val);

    ~Json();

    Json operator[](const std::string& key);

private:
    Json(bool allocate);
    Json(json_object_t* obj);

public:
    int type() const { return json_value_type(root_); }

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

private:
    json_value_t *root_ = nullptr;
    bool allocate_ = false;
};

}  // namespace wfrest

#endif  // WFREST_JSON_H_
