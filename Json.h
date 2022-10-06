#ifndef WFREST_JSON_H_
#define WFREST_JSON_H_

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <functional>
#include <map>
#include "json_parser.h"

namespace wfrest
{

class JsonValue;

class Json
{
public:
    using Object = std::map<std::string, Json>;
public: 
    // Constructors for the various types of JSON value.
    Json();
    Json(std::nullptr_t);
    Json(double value);
    Json(int value); 
    Json(bool value);
    Json(const std::string& str);  
    Json(const char* str);
    Json(const Object& obj);
    Json(JsonValue* val);
    ~Json();

    Json(const Json& json) = delete;
    Json& operator=(const Json& json) = delete;
    Json(Json&& other);
    Json& operator=(Json&& other);

    static Json parse(const std::string &str);
    static Json parse(const std::ifstream& stream);
    
    const std::string dump() const;
    const std::string dump(int spaces) const;

    int type() const;
    bool is_null() const;
    bool is_number() const;
    bool is_boolean() const;
    bool is_object() const;
    bool is_array() const;
    bool is_string() const;

    int size() const; 
    bool empty() const;
    void clear(); 
public:
    // object
    Json& operator[](const std::string& key);
    // todo : need meta programming to optimize
    Json& operator=(int val);
    Json& operator=(double val);
    Json& operator=(bool val);
    Json& operator=(const std::string& val);
    Json& operator=(const char* val);
    Json& operator=(std::nullptr_t val);

    void push_back(const std::string& key, int val);
    void push_back(const std::string& key, double val);
    void push_back(const std::string& key, bool val);
    void push_back(const std::string& key, const std::string& val);
    void push_back(const std::string& key, const char* val);
    void push_back(const std::string& key, std::nullptr_t val);

    void push_back(int val);
    void push_back(double val);
    void push_back(bool val);
    void push_back(const std::string& val);
    void push_back(const char* val);
    void push_back(std::nullptr_t val);
    
private:    
    friend inline std::ostream& operator << (std::ostream& os, const Json& json) { return (os << json.dump()); }

private:
    Object object_;
    std::string key_;
    Json* parent_ = nullptr;  // watcher
    JsonValue* val_ = nullptr;
};

}  // namespace wfrest

#endif  // WFREST_JSON_H_
