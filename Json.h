#ifndef WFREST_JSON_H_
#define WFREST_JSON_H_

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include "json_parser.h"

namespace wfrest
{

class JsonValue;

class Json
{
public:
    using Array = std::vector<Json>;
    using Object = std::map<std::string, Json>;
public: 
    // Constructors for the various types of JSON value.
    Json() = default;  // NULL
    Json(std::nullptr_t);
    Json(double value);
    Json(int value); 
    Json(bool value);
    Json(const std::string& str);  
    Json(const char* str);
    // Json(const Array &values);
    // Json(const Object &values);
    ~Json();

    static Json parse(const std::string &str);
    static Json parse(const std::ifstream& stream);
    
    std::string dump();
    std::string dump(int spaces);

    Json& operator[](const std::string& key);
    Json& operator[](std::string&& key);

    // void operator=(int integer);

private:
    void value_convert(const json_value_t *val, int spaces, int depth, std::string* out_str);

    void string_convert(const char *raw_str, std::string* out_str);

    void number_convert(double number, std::string* out_str);

    void array_convert(const json_array_t *arr, int spaces, int depth, std::string* out_str);

    void array_convert_not_format(const json_array_t *arr, std::string* out_str);

    void object_convert(const json_object_t *obj, int spaces, int depth, std::string* out_str);

    void object_convert_not_format(const json_object_t *obj, std::string* out_str);

private:
    JsonValue* val_ = nullptr;
};

}  // namespace wfrest


#endif  // WFREST_JSON_H_
