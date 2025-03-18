#ifndef JSON_WRAPPER_HPP
#define JSON_WRAPPER_HPP

#include <stdexcept>
#include <string>
#include <sstream>
#include <memory>
#include "json_parser.h"
class Json {
private:
    using JsonValuePtr = std::shared_ptr<json_value_t>;

    JsonValuePtr value;

    std::string dump_value(const json_value_t* val) const {
        if (!val) return "null";
        
        switch (json_value_type(val)) {
            case JSON_VALUE_STRING: {
                const char* str = json_value_string(val);
                return "\"" + std::string(str ? str : "") + "\"";
            }
            case JSON_VALUE_NUMBER: {
                double num = json_value_number(val);
                std::ostringstream oss;
                oss << num;
                return oss.str();
            }
            case JSON_VALUE_TRUE:
                return "true";
            case JSON_VALUE_FALSE:
                return "false";
            case JSON_VALUE_NULL:
                return "null";
            case JSON_VALUE_OBJECT: {
                std::ostringstream oss;
                oss << "{";
                const json_object_t* obj = json_value_object(val);
                const char* name = nullptr;
                const json_value_t* v = nullptr;
                bool first = true;
                json_object_for_each(name, v, obj) {
                    if (!first) oss << ",";
                    oss << "\"" << name << "\":" << dump_value(v);
                    first = false;
                }
                oss << "}";
                return oss.str();
            }
            case JSON_VALUE_ARRAY: {
                std::ostringstream oss;
                oss << "[";
                const json_array_t* arr = json_value_array(val);
                const json_value_t* v = nullptr;
                bool first = true;
                json_array_for_each(v, arr) {
                    if (!first) oss << ",";
                    oss << dump_value(v);
                    first = false;
                }
                oss << "]";
                return oss.str();
            }
            default:
                return "null";
        }
    }

    // 代理类，仅用于赋值
    class Proxy {
    private:
        Json& parent;
        std::string key;  // 对于对象
        size_t index;     // 对于数组
        bool is_array;

    public:
        Proxy(Json& p, const std::string& k) : parent(p), key(k), index(0), is_array(false) {}
        Proxy(Json& p, size_t i) : parent(p), key(""), index(i), is_array(true) {}

        Proxy& operator=(const Json& other) {
            if (is_array) {
                json_array_t* arr = json_value_array(parent.value.get());
                const json_value_t* val = nullptr;
                size_t i = 0;
                const json_value_t* v = nullptr;
                json_array_for_each(v, arr) {
                    if (i++ == index) {
                        val = v;
                        break;
                    }
                }
                if (val) {
                    json_array_remove(val, arr);
                }
                json_value_t* copy = json_value_copy(other.value.get());
                json_array_append(arr, json_value_type(copy), json_value_number(copy));
                json_value_destroy(copy);
            } else {
                json_object_t* obj = json_value_object(parent.value.get());
                const json_value_t* old_val = json_object_find(key.c_str(), obj);
                if (old_val) {
                    json_object_remove(old_val, obj);
                }
                json_value_t* copy = json_value_copy(other.value.get());
                json_object_append(obj, key.c_str(), json_value_type(copy), json_value_number(copy));
                json_value_destroy(copy);
            }
            return *this;
        }

        Proxy& operator=(int val) { return *this = Json(val); }
        Proxy& operator=(double val) { return *this = Json(val); }
        Proxy& operator=(bool val) { return *this = Json(val); }
        Proxy& operator=(const std::string& val) { return *this = Json(val.c_str()); }
        Proxy& operator=(const char* val) { return *this = Json(val); }
        Proxy& operator=(std::nullptr_t) { return *this = Json(nullptr); }
    };

    // 读取时返回 Json 对象
    Json get_element(const std::string& key) const {
        if (!is_object()) {
            throw std::runtime_error("Not an object");
        }
        json_object_t* obj = json_value_object(value.get());
        const json_value_t* val = json_object_find(key.c_str(), obj);
        if (!val) {
            val = json_object_append(obj, key.c_str(), JSON_VALUE_NULL);
        }
        Json result;
        result.value = value;  // 共享所有权
        return result;
    }

    Json get_element(size_t index) const {
        if (!is_array()) {
            throw std::runtime_error("Not an array");
        }
        json_array_t* arr = json_value_array(value.get());
        const json_value_t* val = nullptr;
        size_t i = 0;
        const json_value_t* v = nullptr;
        json_array_for_each(v, arr) {
            if (i++ == index) {
                val = v;
                break;
            }
        }
        if (!val) throw std::out_of_range("Array index out of range");
        Json result;
        result.value = value;  // 共享所有权
        return result;
    }

public:
    Json() : value(json_value_create(JSON_VALUE_OBJECT), json_value_destroy) {}

    explicit Json(const std::string& json_str) 
        : value(json_value_parse(json_str.c_str()), json_value_destroy) {
        if (!value) throw std::runtime_error("Failed to parse JSON string");
    }

    Json(int val) : value(json_value_create(JSON_VALUE_NUMBER, static_cast<double>(val)), json_value_destroy) {}
    Json(double val) : value(json_value_create(JSON_VALUE_NUMBER, val), json_value_destroy) {}
    Json(bool val) : value(json_value_create(val ? JSON_VALUE_TRUE : JSON_VALUE_FALSE), json_value_destroy) {}
    Json(const char* val) : value(json_value_create(JSON_VALUE_STRING, val), json_value_destroy) {}
    Json(std::nullptr_t) : value(json_value_create(JSON_VALUE_NULL), json_value_destroy) {}

    Json(const Json& other) : value(other.value) {}
    Json(Json&& other) noexcept : value(std::move(other.value)) {}

    Json& operator=(const Json& other) {
        if (this != &other) {
            value = other.value;
        }
        return *this;
    }

    bool is_object() const { return json_value_type(value.get()) == JSON_VALUE_OBJECT; }
    bool is_array() const { return json_value_type(value.get()) == JSON_VALUE_ARRAY; }
    bool is_string() const { return json_value_type(value.get()) == JSON_VALUE_STRING; }
    bool is_number() const { return json_value_type(value.get()) == JSON_VALUE_NUMBER; }
    bool is_boolean() const { return json_value_type(value.get()) == JSON_VALUE_TRUE || 
                                   json_value_type(value.get()) == JSON_VALUE_FALSE; }
    bool is_null() const { return json_value_type(value.get()) == JSON_VALUE_NULL; }

    // 区分赋值和读取
    Proxy operator[](const std::string& key) { return Proxy(*this, key); }
    Proxy operator[](size_t index) { return Proxy(*this, index); }

    // 提供读取接口
    Json at(const std::string& key) const { return get_element(key); }
    Json at(size_t index) const { return get_element(index); }

    template<typename T>
    T get() const;

    void push_back(const Json& item) {
        if (!is_array()) {
            value.reset(json_value_create(JSON_VALUE_ARRAY), json_value_destroy);
        }
        json_array_t* arr = json_value_array(value.get());
        json_value_t* copy = json_value_copy(item.value.get());
        switch (json_value_type(copy)) {
            case JSON_VALUE_STRING:
                json_array_append(arr, JSON_VALUE_STRING, json_value_string(copy));
                break;
            case JSON_VALUE_NUMBER:
                json_array_append(arr, JSON_VALUE_NUMBER, json_value_number(copy));
                break;
            case JSON_VALUE_TRUE:
            case JSON_VALUE_FALSE:
                json_array_append(arr, json_value_type(copy));
                break;
            case JSON_VALUE_NULL:
                json_array_append(arr, JSON_VALUE_NULL);
                break;
            default:
                json_array_append(arr, json_value_type(copy), copy);
                break;
        }
        if (copy) json_value_destroy(copy);
    }

    size_t size() const {
        if (is_array()) return json_array_size(json_value_array(value.get()));
        if (is_object()) return json_object_size(json_value_object(value.get()));
        return 0;
    }

    bool has(const std::string& key) const {
        if (!is_object()) return false;
        return json_object_find(key.c_str(), json_value_object(value.get())) != nullptr;
    }

    std::string dump() const {
        return dump_value(value.get());
    }

    static Json parse(const std::string& str) {
        return Json(str);
    }

private:
    Json(JsonValuePtr ptr) : value(ptr) {}
};

template<>
inline int Json::get<int>() const {
    if (!is_number()) throw std::runtime_error("JSON value is not a number");
    return static_cast<int>(json_value_number(value.get()));
}

template<>
inline double Json::get<double>() const {
    if (!is_number()) throw std::runtime_error("JSON value is not a number");
    return json_value_number(value.get());
}

template<>
inline bool Json::get<bool>() const {
    if (!is_boolean()) throw std::runtime_error("JSON value is not a boolean");
    return json_value_type(value.get()) == JSON_VALUE_TRUE;
}

template<>
inline std::string Json::get<std::string>() const {
    if (!is_string()) throw std::runtime_error("JSON value is not a string");
    return std::string(json_value_string(value.get()));
}

#endif