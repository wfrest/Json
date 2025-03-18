#ifndef _JSON_H_
#define _JSON_H_

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <memory>
#include <type_traits>
#include <functional>
#include <utility>
#include <iterator>
#include <cassert>

#include "json_parser.h"

namespace wfrest {

// 前向声明
class Json;

// Global operator<< for ostream
std::ostream& operator<<(std::ostream& os, const Json& json);

class Json {
public:
    class Object;
    class Array;
    class Iterator;
    class ReverseIterator;

    // Constructors
    Json();  // Default constructor creates a null value
    Json(std::nullptr_t);
    Json(int value);
    Json(double value);
    Json(bool value);
    Json(const char* value);
    Json(const std::string& value);
    Json(const Object& obj);
    Json(const Array& arr);
    
    // Copy and move operations
    Json(const Json& other);
    Json(Json&& other) noexcept;
    Json& operator=(const Json& other);
    Json& operator=(Json&& other) noexcept;
    
    // 基本类型赋值操作符
    Json& operator=(int value);
    Json& operator=(double value);
    Json& operator=(bool value);
    Json& operator=(const char* value);
    Json& operator=(const std::string& value);
    Json& operator=(std::nullptr_t);
    Json& operator=(const Object& obj);
    Json& operator=(const Array& arr);

    // Static factory methods
    static Json parse(const std::string& json_str);
    static Json parse(std::ifstream& file);
    static Json parse(FILE* fp);
    static Object object();
    static Array array();

    // Type checking
    int type() const;
    bool is_null() const;
    bool is_boolean() const;
    bool is_number() const;
    bool is_string() const;
    bool is_object() const;
    bool is_array() const;
    bool is_valid() const;

    // Value getters with type checking
    template<typename T>
    T get() const;

    // Implicit conversion operators
    operator int() const;
    operator double() const;
    operator bool() const;
    operator std::string() const;
    operator std::nullptr_t() const;
    
    // Get Object/Array - since direct operator conversion causes issues
    Object to_object() const;
    Array to_array() const;

    // Object operations
    Json& operator[](const std::string& key);
    const Json& operator[](const std::string& key) const;
    Json& operator[](const char* key);
    const Json& operator[](const char* key) const;
    void push_back(const std::string& key, const Json& value);
    void push_back(const char* key, const Json& value);
    void erase(const std::string& key);
    void erase(const char* key);
    bool has(const std::string& key) const;
    bool has(const char* key) const;
    bool empty() const;
    size_t size() const;
    void clear();
    
    // Array operations
    Json& operator[](size_t index);
    const Json& operator[](size_t index) const;
    void push_back(const Json& value);
    void erase(size_t index);

    // Serialization
    std::string dump(int indent = -1) const;

    // Copy operation
    Json copy() const;

    // Iterator support
    using iterator = Iterator;
    using reverse_iterator = ReverseIterator;
    
    iterator begin();
    iterator end();
    reverse_iterator rbegin();
    reverse_iterator rend();

    // Destructor
    ~Json();

private:
    json_value_t* value_;
    bool valid_;
    bool owns_value_; // New flag to indicate ownership of the value_

    // Reference tracking for memory management
    static std::vector<Json*>& reference_registry();
    static void register_reference(Json* ref);
    static void cleanup_references();
    static void cleanup_all_references();

    // Constructors that take ownership of a json_value_t
    explicit Json(json_value_t* value, bool take_ownership = true);
    
    // Helper method to create a reference to a child node
    Json& create_reference(json_value_t* child_value);
    
    // Static null value for error cases
    static Json& null_value();

    friend std::ostream& operator<<(std::ostream& os, const Json& json);
    friend class Object;
    friend class Array;
    friend class Iterator;
    friend class ReverseIterator;
};

// Wrapper class for JSON objects
class Json::Object {
public:
    Object();
    
    // Element access
    Json& operator[](const std::string& key);
    Json& operator[](const char* key);
    
    // Modifiers
    void push_back(const std::string& key, const Json& value);
    void push_back(const char* key, const Json& value);
    void erase(const std::string& key);
    void erase(const char* key);
    bool has(const std::string& key) const;
    bool has(const char* key) const;
    void clear();
    
    // Capacity
    bool empty() const;
    size_t size() const;
    
    // String conversion
    std::string dump(int indent = -1) const;
    
    // Internal conversions
    operator Json() const;
    json_object_t* native_object() const;
    
private:
    Json json_;
    
    friend class Json;
};

// Wrapper class for JSON arrays
class Json::Array {
public:
    Array();
    
    // Element access
    Json& operator[](size_t index);
    
    // Modifiers
    void push_back(const Json& value);
    void erase(size_t index);
    void clear();
    
    // Capacity
    bool empty() const;
    size_t size() const;
    
    // String conversion
    std::string dump(int indent = -1) const;
    
    // Internal conversions
    operator Json() const;
    json_array_t* native_array() const;
    
private:
    Json json_;
    
    friend class Json;
};

// Iterator implementation for both objects and arrays
class Json::Iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Json;
    using difference_type = std::ptrdiff_t;
    using pointer = Json*;
    using reference = Json&;

    Iterator(const Json* json, const json_value_t* value = nullptr, const char* name = nullptr);
    
    // Copy and move operations
    Iterator(const Iterator& other);
    Iterator(Iterator&& other) noexcept;
    Iterator& operator=(const Iterator& other);
    Iterator& operator=(Iterator&& other) noexcept;
    
    // Iterator operations
    Iterator& operator++();
    Iterator operator++(int);
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;
    
    // Dereference operations
    const Json& operator*() const;
    const Json* operator->() const;
    
    // Special accessor for objects
    std::string key() const;
    const Json& value() const;

private:
    const Json* json_;
    const json_value_t* current_value_;
    const char* current_name_;
};

// ReverseIterator implementation for both objects and arrays
class Json::ReverseIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Json;
    using difference_type = std::ptrdiff_t;
    using pointer = Json*;
    using reference = Json&;

    ReverseIterator(const Json* json, const json_value_t* value = nullptr, const char* name = nullptr);
    
    // Copy and move operations
    ReverseIterator(const ReverseIterator& other);
    ReverseIterator(ReverseIterator&& other) noexcept;
    ReverseIterator& operator=(const ReverseIterator& other);
    ReverseIterator& operator=(ReverseIterator&& other) noexcept;
    
    // Iterator operations
    ReverseIterator& operator++();
    ReverseIterator operator++(int);
    bool operator==(const ReverseIterator& other) const;
    bool operator!=(const ReverseIterator& other) const;
    
    // Dereference operations
    const Json& operator*() const;
    const Json* operator->() const;
    
    // Special accessor for objects
    std::string key() const;
    const Json& value() const;

private:
    const Json* json_;
    const json_value_t* current_value_;
    const char* current_name_;
};

// Template specializations for get<T>
template<>
inline int Json::get<int>() const {
    if (type() != JSON_VALUE_NUMBER) {
        return 0;
    }
    return static_cast<int>(json_value_number(value_));
}

template<>
inline double Json::get<double>() const {
    if (type() != JSON_VALUE_NUMBER) {
        return 0.0;
    }
    return json_value_number(value_);
}

template<>
inline bool Json::get<bool>() const {
    if (type() == JSON_VALUE_TRUE) {
        return true;
    } else if (type() == JSON_VALUE_FALSE) {
        return false;
    }
    return false;
}

template<>
inline std::string Json::get<std::string>() const {
    if (type() != JSON_VALUE_STRING) {
        return "";
    }
    const char* str = json_value_string(value_);
    return str ? str : "";
}

template<>
inline std::nullptr_t Json::get<std::nullptr_t>() const {
    return nullptr;
}

// 将get<Object>和get<Array>的特化移到.cc文件中实现

} // namespace wfrest

#endif // _JSON_H_
