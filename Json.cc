#include "Json.h"
#include <sstream>
#include <cstring>
#include <mutex>
#include <algorithm>

namespace wfrest {

// Reference tracking implementation
std::vector<Json*>& Json::reference_registry() {
    static std::mutex init_mutex;
    static std::vector<Json*>* registry = nullptr;
    
    // Double-checked locking pattern for initialization
    if (!registry) {
        std::lock_guard<std::mutex> lock(init_mutex);
        if (!registry) {
            registry = new std::vector<Json*>();
        }
    }
    return *registry;
}

void Json::register_reference(Json* ref) {
    static std::mutex registry_mutex;
    std::lock_guard<std::mutex> lock(registry_mutex);
    reference_registry().push_back(ref);
}

void Json::cleanup_references() {
    static std::mutex registry_mutex;
    std::lock_guard<std::mutex> lock(registry_mutex);
    
    // 创建一个临时副本，避免在迭代过程中修改容器
    auto refs = reference_registry();
    
    // 清空原注册表，避免在删除过程中再次调用 cleanup_references
    reference_registry().clear();
    
    // 删除所有注册的引用
    for (Json* ref : refs) {
        delete ref;
    }
}

// Add a method to clean up references in the destructor more efficiently
void Json::cleanup_all_references() {
    // 不再需要调用 cleanup_references，直接简化为：
    static std::mutex registry_mutex;
    std::lock_guard<std::mutex> lock(registry_mutex);
    
    if (!reference_registry().empty()) {
        // 创建一个临时副本，避免在迭代过程中修改容器
        auto refs = reference_registry();
        
        // 清空原注册表，避免在删除过程中再次调用
        reference_registry().clear();
        
        // 删除所有注册的引用
        for (Json* ref : refs) {
            delete ref;
        }
    }
}

// Json class implementation
Json::Json() : value_(json_value_create(JSON_VALUE_NULL)), valid_(true), owns_value_(true) {}

Json::Json(std::nullptr_t) : value_(json_value_create(JSON_VALUE_NULL)), valid_(true), owns_value_(true) {}

Json::Json(int value) : value_(json_value_create(JSON_VALUE_NUMBER, static_cast<double>(value))), valid_(true), owns_value_(true) {}

Json::Json(double value) : value_(json_value_create(JSON_VALUE_NUMBER, value)), valid_(true), owns_value_(true) {}

Json::Json(bool value) : value_(json_value_create(value ? JSON_VALUE_TRUE : JSON_VALUE_FALSE)), valid_(true), owns_value_(true) {}

Json::Json(const char* value) : value_(json_value_create(JSON_VALUE_STRING, value)), valid_(true), owns_value_(true) {}

Json::Json(const std::string& value) : value_(json_value_create(JSON_VALUE_STRING, value.c_str())), valid_(true), owns_value_(true) {}

Json::Json(const Object& obj) : valid_(true), owns_value_(true) {
    value_ = json_value_copy(obj.json_.value_);
}

Json::Json(const Array& arr) : valid_(true), owns_value_(true) {
    value_ = json_value_copy(arr.json_.value_);
}

// Copy constructor
Json::Json(const Json& other) : valid_(other.valid_), owns_value_(true) {
    value_ = other.value_ ? json_value_copy(other.value_) : nullptr;
}

// Move constructor
Json::Json(Json&& other) noexcept : value_(other.value_), valid_(other.valid_), owns_value_(other.owns_value_) {
    other.value_ = nullptr;
    other.owns_value_ = false;
}

// Copy assignment operator
Json& Json::operator=(const Json& other) {
    if (this != &other) {
        if (value_ && owns_value_) {
            json_value_destroy(value_);
        }
        
        // Ensure we get a proper deep copy with correct type information
        if (other.value_) {
            value_ = json_value_copy(other.value_);
        } else {
            value_ = json_value_create(JSON_VALUE_NULL);
        }
        
        valid_ = other.valid_;
        owns_value_ = true;
    }
    return *this;
}

// Move assignment operator
Json& Json::operator=(Json&& other) noexcept {
    if (this != &other) {
        if (value_ && owns_value_) {
            json_value_destroy(value_);
        }
        value_ = other.value_;
        valid_ = other.valid_;
        owns_value_ = other.owns_value_;
        other.value_ = nullptr;
        other.owns_value_ = false;
    }
    return *this;
}

Json::Json(json_value_t* value, bool take_ownership) : value_(value), valid_(true), owns_value_(take_ownership) {}

Json::~Json() {
    if (value_ && owns_value_) {
        json_value_destroy(value_);
        value_ = nullptr;
    }
    
    // 仅当这是顶层对象（拥有值的对象）时清理引用
    // 同时设置一个标记，确保仅清理一次
    static thread_local bool cleaning_references = false;
    if (owns_value_ && !cleaning_references) {
        cleaning_references = true;
        cleanup_all_references();
        cleaning_references = false;
    }
}

// Helper method to create non-owning reference to a child node
Json& Json::create_reference(json_value_t* child_value) {
    // Create a new reference that doesn't own the value
    Json* ref = new Json(child_value, false);
    
    // Register the reference for cleanup
    register_reference(ref);
    
    return *ref;
}

// Static null value for error cases
Json& Json::null_value() {
    static std::mutex null_mutex;
    static Json* null_instance = nullptr;
    
    std::lock_guard<std::mutex> lock(null_mutex);
    if (!null_instance) {
        null_instance = new Json();
    }
    return *null_instance;
}

// C-style string overloads implementation
Json& Json::operator[](const char* key) {
    return operator[](std::string(key));
}

const Json& Json::operator[](const char* key) const {
    return operator[](std::string(key));
}

void Json::push_back(const char* key, const Json& value) {
    push_back(std::string(key), value);
}

void Json::erase(const char* key) {
    erase(std::string(key));
}

bool Json::has(const char* key) const {
    return has(std::string(key));
}

// Static factory methods
Json Json::parse(const std::string& json_str) {
    json_value_t* value = json_value_parse(json_str.c_str());
    if (!value) {
        Json invalid;
        invalid.valid_ = false;
        return invalid;
    }
    return Json(value, true); // Explicitly take ownership
}

Json Json::parse(std::ifstream& file) {
    std::stringstream ss;
    ss << file.rdbuf();
    return parse(ss.str());
}

Json Json::parse(FILE* fp) {
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    std::string buffer;
    buffer.resize(length);
    
    size_t read_size = fread(&buffer[0], 1, length, fp);
    buffer.resize(read_size); // Adjust to actual read size
    
    return parse(buffer);
}

Json::Object Json::object() {
    return Json::Object();
}

Json::Array Json::array() {
    return Json::Array();
}

// Type checking
int Json::type() const {
    return value_ ? json_value_type(value_) : JSON_VALUE_NULL;
}

bool Json::is_null() const {
    return type() == JSON_VALUE_NULL;
}

bool Json::is_boolean() const {
    return type() == JSON_VALUE_TRUE || type() == JSON_VALUE_FALSE;
}

bool Json::is_number() const {
    if (!value_) {
        return false;
    }
    return json_value_type(value_) == JSON_VALUE_NUMBER;
}

bool Json::is_string() const {
    return type() == JSON_VALUE_STRING;
}

bool Json::is_object() const {
    return type() == JSON_VALUE_OBJECT;
}

bool Json::is_array() const {
    return type() == JSON_VALUE_ARRAY;
}

bool Json::is_valid() const {
    return valid_;
}

// Implicit conversion operators
Json::operator int() const {
    return get<int>();
}

Json::operator double() const {
    return get<double>();
}

Json::operator bool() const {
    return get<bool>();
}

Json::operator std::string() const {
    return get<std::string>();
}

Json::operator std::nullptr_t() const {
    return nullptr;
}

// Template specializations are already in header file

Json::Object Json::to_object() const {
    if (!is_object()) {
        return Object();
    }
    
    Object obj;
    obj.json_ = *this;
    return obj;
}

Json::Array Json::to_array() const {
    if (!is_array()) {
        return Array();
    }
    
    Array arr;
    arr.json_ = *this;
    return arr;
}

// Object operations
Json& Json::operator[](const std::string& key) {
    if (!is_object()) {
        if (is_null()) {
            // Convert null to object
            if (owns_value_) {
                json_value_destroy(value_);
            }
            value_ = json_value_create(JSON_VALUE_OBJECT);
            owns_value_ = true;
        } else {
            // Cannot convert non-null, non-object to object
            return null_value();
        }
    }
    
    json_object_t* obj = json_value_object(value_);
    const json_value_t* found = json_object_find(key.c_str(), obj);
    
    if (found) {
        // Return a non-owning reference to the found value
        // The value remains owned by the object
        return create_reference(const_cast<json_value_t*>(found));
    } else {
        // Create a null value and add it to the object
        // Using type 0 means we use new_value directly instead of creating a new value
        json_value_t* new_value = json_value_create(JSON_VALUE_NULL);
        json_object_append(obj, key.c_str(), 0, new_value);
        
        // Get the inserted value and return a reference to it
        const json_value_t* inserted = json_object_find(key.c_str(), obj);
        
        // Create a non-owning reference - value is owned by the object
        Json* ref = new Json(const_cast<json_value_t*>(inserted), false);
        
        // Register the reference for cleanup
        register_reference(ref);
        
        return *ref;
    }
}

const Json& Json::operator[](const std::string& key) const {
    if (!is_object()) {
        return null_value();
    }
    
    json_object_t* obj = json_value_object(value_);
    const json_value_t* found = json_object_find(key.c_str(), obj);
    
    if (found) {
        // Instead of using thread_local, create a heap-allocated copy
        // that will be managed by reference_registry
        Json* result = new Json(json_value_copy(const_cast<json_value_t*>(found)), true);
        register_reference(result);
        return *result;
    } else {
        return null_value();
    }
}

void Json::push_back(const std::string& key, const Json& value) {
    if (!is_object()) {
        if (is_null()) {
            // Convert null to object
            if (owns_value_ && value_) {
                json_value_destroy(value_);
            }
            value_ = json_value_create(JSON_VALUE_OBJECT);
            owns_value_ = true;
        } else {
            // Cannot convert non-null, non-object to object
            return;
        }
    }
    
    json_object_t* obj = json_value_object(value_);
    
    // Always create a deep copy of the value being added
    json_value_t* copied_value = json_value_copy(value.value_);
    
    // Remove any existing key with the same name
    const json_value_t* found = json_object_find(key.c_str(), obj);
    if (found) {
        json_value_t* removed = json_object_remove(found, obj);
        if (removed) {
            json_value_destroy(removed);
        }
    }
    
    // Append the new value directly with type 0 to prevent re-wrapping
    json_object_append(obj, key.c_str(), 0, copied_value);
    
    // Clean up any existing references that might be invalidated
    // by modifying the object
    cleanup_references();
}

void Json::erase(const std::string& key) {
    if (!is_object()) {
        return;
    }
    
    json_object_t* obj = json_value_object(value_);
    const json_value_t* found = json_object_find(key.c_str(), obj);
    
    if (found) {
        json_value_t* removed = json_object_remove(found, obj);
        if (removed) {
            json_value_destroy(removed);
        }
        
        // Clean up any existing references that might be invalidated
        cleanup_references();
    }
}

bool Json::has(const std::string& key) const {
    if (!is_object()) {
        return false;
    }
    
    json_object_t* obj = json_value_object(value_);
    return json_object_find(key.c_str(), obj) != nullptr;
}

bool Json::empty() const {
    if (is_object()) {
        json_object_t* obj = json_value_object(value_);
        return json_object_size(obj) == 0;
    } else if (is_array()) {
        json_array_t* arr = json_value_array(value_);
        return json_array_size(arr) == 0;
    }
    return true;
}

size_t Json::size() const {
    if (is_object()) {
        json_object_t* obj = json_value_object(value_);
        return json_object_size(obj);
    } else if (is_array()) {
        json_array_t* arr = json_value_array(value_);
        return json_array_size(arr);
    }
    return 0;
}

void Json::clear() {
    if (is_object()) {
        json_object_t* obj = json_value_object(value_);
        const char* name = NULL;
        const json_value_t* val = NULL;
        
        std::vector<std::string> keys;
        
        json_object_for_each(name, val, obj) {
            keys.push_back(name);
        }
        
        for (const auto& key : keys) {
            erase(key);
        }
        
        // Clean up any references that might be referring to this object
        cleanup_references();
    } else if (is_array()) {
        // For arrays, recreate as empty array
        if (owns_value_ && value_) {
            json_value_destroy(value_);
        }
        value_ = json_value_create(JSON_VALUE_ARRAY);
        owns_value_ = true;
        
        // Clean up references
        cleanup_references();
    }
}

// Array operations
Json& Json::operator[](size_t index) {
    if (!is_array()) {
        if (is_null()) {
            // Convert null to array
            json_value_destroy(value_);
            value_ = json_value_create(JSON_VALUE_ARRAY);
        } else {
            // Cannot convert non-null, non-array to array
            return null_value();
        }
    }
    
    json_array_t* arr = json_value_array(value_);
    
    // Ensure array has enough elements
    size_t current_size = json_array_size(arr);
    if (index >= current_size) {
        for (size_t i = current_size; i <= index; ++i) {
            json_array_append(arr, JSON_VALUE_NULL);
        }
    }
    
    // Find element at index
    const json_value_t* val = NULL;
    size_t i = 0;
    
    json_array_for_each(val, arr) {
        if (i == index) {
            // Return a non-owning reference to the array element
            return create_reference(const_cast<json_value_t*>(val));
        }
        i++;
    }
    
    // Should never reach here
    return null_value();
}

const Json& Json::operator[](size_t index) const {
    if (!is_array()) {
        return null_value();
    }
    
    json_array_t* arr = json_value_array(value_);
    
    // Check if index is in bounds
    size_t current_size = json_array_size(arr);
    if (index >= current_size) {
        return null_value();
    }
    
    // Find element at index
    const json_value_t* val = NULL;
    size_t i = 0;
    
    json_array_for_each(val, arr) {
        if (i == index) {
            // Create a heap-allocated copy that will be managed by reference_registry
            Json* result = new Json(json_value_copy(const_cast<json_value_t*>(val)), true);
            register_reference(result);
            return *result;
        }
        i++;
    }
    
    // Should never reach here
    return null_value();
}

void Json::push_back(const Json& value) {
    if (!is_array()) {
        if (is_null()) {
            // Convert null to array
            if (owns_value_ && value_) {
                json_value_destroy(value_);
            }
            value_ = json_value_create(JSON_VALUE_ARRAY);
            owns_value_ = true;
        } else {
            // Cannot convert non-null, non-array to array
            return;
        }
    }
    
    json_array_t* arr = json_value_array(value_);
    
    // Create a deep copy of the value with correct type information
    json_value_t* copied_value = json_value_copy(value.value_);
    
    // Add the copy to the array using type 0 to use copied_value directly
    json_array_append(arr, 0, copied_value);
    
    // Clean up any references that might be invalidated
    cleanup_references();
}

void Json::erase(size_t index) {
    if (!is_array()) {
        return;
    }
    
    json_array_t* arr = json_value_array(value_);
    
    // Check if index is in bounds
    size_t current_size = json_array_size(arr);
    if (index >= current_size) {
        return;
    }
    
    // Find element at index
    const json_value_t* val = NULL;
    size_t i = 0;
    
    json_array_for_each(val, arr) {
        if (i == index) {
            json_value_t* removed = json_array_remove(val, arr);
            if (removed) {
                json_value_destroy(removed);
            }
            
            // Clean up any existing references that might be invalidated
            cleanup_references();
            return;
        }
        i++;
    }
}

// Serialization
std::string Json::dump(int indent) const {
    if (!value_) {
        return "null";
    }

    switch (type()) {
        case JSON_VALUE_NULL:
            return "null";
        case JSON_VALUE_TRUE:
            return "true";
        case JSON_VALUE_FALSE:
            return "false";
        case JSON_VALUE_NUMBER:
        {
            double num = json_value_number(value_);
            // Handle integer vs float formatting
            if (num == (int)num) {
                return std::to_string((int)num);
            } else {
                std::ostringstream ss;
                ss << num;
                return ss.str();
            }
        }
        case JSON_VALUE_STRING:
        {
            std::string result = "\"";
            const char* str = json_value_string(value_);
            if (str) {
                for (const char* p = str; *p; ++p) {
                    switch (*p) {
                        case '\"': result += "\\\""; break;
                        case '\\': result += "\\\\"; break;
                        case '\b': result += "\\b"; break;
                        case '\f': result += "\\f"; break;
                        case '\n': result += "\\n"; break;
                        case '\r': result += "\\r"; break;
                        case '\t': result += "\\t"; break;
                        default:
                            if ((unsigned char)*p < 0x20) {
                                char buf[7];
                                snprintf(buf, sizeof(buf), "\\u%04x", (unsigned char)*p);
                                result += buf;
                            } else {
                                result += *p;
                            }
                    }
                }
            }
            result += "\"";
            return result;
        }
        case JSON_VALUE_OBJECT:
        {
            std::string result = "{";
            json_object_t* obj = json_value_object(value_);
            const char* name = NULL;
            const json_value_t* val = NULL;
            bool first = true;
            
            json_object_for_each(name, val, obj) {
                if (!first) {
                    result += ",";
                }
                first = false;
                
                if (indent >= 0) {
                    result += "\n" + std::string(indent + 2, ' ');
                }
                
                // Key with quotes
                result += "\"" + std::string(name) + "\"";
                result += ":";
                
                if (indent >= 0) {
                    result += " ";
                }
                
                // Create a non-owning temporary Json for the value
                Json tmp(const_cast<json_value_t*>(val), false);
                result += tmp.dump(indent >= 0 ? indent + 2 : -1);
            }
            
            if (!first && indent >= 0) {
                result += "\n" + std::string(indent, ' ');
            }
            
            result += "}";
            return result;
        }
        case JSON_VALUE_ARRAY:
        {
            std::string result = "[";
            json_array_t* arr = json_value_array(value_);
            const json_value_t* val = NULL;
            bool first = true;
            
            json_array_for_each(val, arr) {
                if (!first) {
                    result += ",";
                }
                first = false;
                
                if (indent >= 0) {
                    result += "\n" + std::string(indent + 2, ' ');
                }
                
                // Create a non-owning temporary Json for the value
                Json tmp(const_cast<json_value_t*>(val), false);
                result += tmp.dump(indent >= 0 ? indent + 2 : -1);
            }
            
            if (!first && indent >= 0) {
                result += "\n" + std::string(indent, ' ');
            }
            
            result += "]";
            return result;
        }
        default:
            return "null";
    }
}

// Copy operation
Json Json::copy() const {
    if (!value_) {
        return Json();
    }
    
    json_value_t* copy = json_value_copy(value_);
    return Json(copy);
}

// Iterator support
Json::iterator Json::begin() {
    if (is_object()) {
        json_object_t* obj = json_value_object(value_);
        const char* name = json_object_next_name(NULL, obj);
        const json_value_t* val = json_object_next_value(NULL, obj);
        return Iterator(this, val, name);
    } else if (is_array()) {
        json_array_t* arr = json_value_array(value_);
        const json_value_t* val = json_array_next_value(NULL, arr);
        return Iterator(this, val);
    }
    return end();
}

Json::iterator Json::end() {
    return Iterator(this);
}

Json::reverse_iterator Json::rbegin() {
    if (is_object()) {
        json_object_t* obj = json_value_object(value_);
        const char* name = json_object_prev_name(NULL, obj);
        const json_value_t* val = json_object_prev_value(NULL, obj);
        return ReverseIterator(this, val, name);
    } else if (is_array()) {
        json_array_t* arr = json_value_array(value_);
        const json_value_t* val = json_array_prev_value(NULL, arr);
        return ReverseIterator(this, val);
    }
    return rend();
}

Json::reverse_iterator Json::rend() {
    return ReverseIterator(this);
}

// Object implementation
Json::Object::Object() : json_(json_value_create(JSON_VALUE_OBJECT)) {}


Json& Json::Object::operator[](const std::string& key) {
    return json_[key];
}

// C-style string overload for Object class
Json& Json::Object::operator[](const char* key) {
    return json_[key];
}

void Json::Object::push_back(const std::string& key, const Json& value) {
    json_.push_back(key, value);
}

// C-style string overload for Object::push_back
void Json::Object::push_back(const char* key, const Json& value) {
    json_.push_back(key, value);
}

void Json::Object::erase(const std::string& key) {
    json_.erase(key);
}

// C-style string overload for Object::erase
void Json::Object::erase(const char* key) {
    json_.erase(key);
}

bool Json::Object::has(const std::string& key) const {
    return json_.has(key);
}

// C-style string overload for Object::has
bool Json::Object::has(const char* key) const {
    return json_.has(key);
}

void Json::Object::clear() {
    json_.clear();
}

bool Json::Object::empty() const {
    return json_.empty();
}

size_t Json::Object::size() const {
    return json_.size();
}

std::string Json::Object::dump(int indent) const {
    return json_.dump(indent);
}

Json::Object::operator Json() const {
    return json_;
}

json_object_t* Json::Object::native_object() const {
    return json_value_object(json_.value_);
}

// Array implementation
Json::Array::Array() : json_(json_value_create(JSON_VALUE_ARRAY)) {}


Json& Json::Array::operator[](size_t index) {
    return json_[index];
}

void Json::Array::push_back(const Json& value) {
    json_.push_back(value);
}

void Json::Array::erase(size_t index) {
    json_.erase(index);
}

void Json::Array::clear() {
    json_.clear();
}

bool Json::Array::empty() const {
    return json_.empty();
}

size_t Json::Array::size() const {
    return json_.size();
}

std::string Json::Array::dump(int indent) const {
    return json_.dump(indent);
}

Json::Array::operator Json() const {
    return json_;
}

json_array_t* Json::Array::native_array() const {
    return json_value_array(json_.value_);
}

// Iterator implementation
Json::Iterator::Iterator(const Json* json, const json_value_t* value, const char* name)
    : json_(json), current_value_(value), current_name_(name) {}

// Copy constructor
Json::Iterator::Iterator(const Iterator& other)
    : json_(other.json_), current_value_(other.current_value_), current_name_(other.current_name_) {}

// Move constructor
Json::Iterator::Iterator(Iterator&& other) noexcept
    : json_(other.json_), current_value_(other.current_value_), current_name_(other.current_name_) {
    other.json_ = nullptr;
    other.current_value_ = nullptr;
    other.current_name_ = nullptr;
}

// Copy assignment operator
Json::Iterator& Json::Iterator::operator=(const Iterator& other) {
    if (this != &other) {
        json_ = other.json_;
        current_value_ = other.current_value_;
        current_name_ = other.current_name_;
    }
    return *this;
}

// Move assignment operator
Json::Iterator& Json::Iterator::operator=(Iterator&& other) noexcept {
    if (this != &other) {
        json_ = other.json_;
        current_value_ = other.current_value_;
        current_name_ = other.current_name_;

        other.json_ = nullptr;
        other.current_value_ = nullptr;
        other.current_name_ = nullptr;
    }
    return *this;
}

Json::Iterator& Json::Iterator::operator++() {
    if (!json_ || !current_value_) {
        return *this;
    }
    
    if (json_->is_object()) {
        json_object_t* obj = json_value_object(json_->value_);
        current_name_ = json_object_next_name(current_name_, obj);
        current_value_ = json_object_next_value(current_value_, obj);
    } else if (json_->is_array()) {
        json_array_t* arr = json_value_array(json_->value_);
        current_value_ = json_array_next_value(current_value_, arr);
    }
    
    return *this;
}

Json::Iterator Json::Iterator::operator++(int) {
    Iterator tmp = *this;
    ++(*this);
    return tmp;
}

bool Json::Iterator::operator==(const Iterator& other) const {
    return json_ == other.json_ && current_value_ == other.current_value_;
}

bool Json::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}

const Json& Json::Iterator::operator*() const {
    if (!current_value_) {
        return Json::null_value();
    }
    
    // Create a heap-allocated copy that will be managed by reference_registry
    Json* result = new Json(json_value_copy(const_cast<json_value_t*>(current_value_)), true);
    Json::register_reference(result);
    return *result;
}

const Json* Json::Iterator::operator->() const {
    if (!current_value_) {
        return &Json::null_value();
    }
    
    // Create a heap-allocated copy that will be managed by reference_registry
    Json* result = new Json(json_value_copy(const_cast<json_value_t*>(current_value_)), true);
    Json::register_reference(result);
    return result;
}

std::string Json::Iterator::key() const {
    return current_name_ ? current_name_ : "";
}

const Json& Json::Iterator::value() const {
    return operator*();
}

// ReverseIterator implementation
Json::ReverseIterator::ReverseIterator(const Json* json, const json_value_t* value, const char* name)
    : json_(json), current_value_(value), current_name_(name) {}

// Copy constructor
Json::ReverseIterator::ReverseIterator(const ReverseIterator& other)
    : json_(other.json_), current_value_(other.current_value_), current_name_(other.current_name_) {}

// Move constructor
Json::ReverseIterator::ReverseIterator(ReverseIterator&& other) noexcept
    : json_(other.json_), current_value_(other.current_value_), current_name_(other.current_name_) {
    other.json_ = nullptr;
    other.current_value_ = nullptr;
    other.current_name_ = nullptr;
}

// Copy assignment operator
Json::ReverseIterator& Json::ReverseIterator::operator=(const ReverseIterator& other) {
    if (this != &other) {
        json_ = other.json_;
        current_value_ = other.current_value_;
        current_name_ = other.current_name_;
    }
    return *this;
}

// Move assignment operator
Json::ReverseIterator& Json::ReverseIterator::operator=(ReverseIterator&& other) noexcept {
    if (this != &other) {
        json_ = other.json_;
        current_value_ = other.current_value_;
        current_name_ = other.current_name_;

        other.json_ = nullptr;
        other.current_value_ = nullptr;
        other.current_name_ = nullptr;
    }
    return *this;
}

Json::ReverseIterator& Json::ReverseIterator::operator++() {
    if (!json_ || !current_value_) {
        return *this;
    }
    
    if (json_->is_object()) {
        json_object_t* obj = json_value_object(json_->value_);
        current_name_ = json_object_prev_name(current_name_, obj);
        current_value_ = json_object_prev_value(current_value_, obj);
    } else if (json_->is_array()) {
        json_array_t* arr = json_value_array(json_->value_);
        current_value_ = json_array_prev_value(current_value_, arr);
    }
    
    return *this;
}

Json::ReverseIterator Json::ReverseIterator::operator++(int) {
    ReverseIterator tmp = *this;
    ++(*this);
    return tmp;
}

bool Json::ReverseIterator::operator==(const ReverseIterator& other) const {
    return json_ == other.json_ && current_value_ == other.current_value_;
}

bool Json::ReverseIterator::operator!=(const ReverseIterator& other) const {
    return !(*this == other);
}

const Json& Json::ReverseIterator::operator*() const {
    if (!current_value_) {
        return Json::null_value();
    }
    
    // Create a heap-allocated copy that will be managed by reference_registry
    Json* result = new Json(json_value_copy(const_cast<json_value_t*>(current_value_)), true);
    Json::register_reference(result);
    return *result;
}

const Json* Json::ReverseIterator::operator->() const {
    if (!current_value_) {
        return &Json::null_value();
    }
    
    // Create a heap-allocated copy that will be managed by reference_registry
    Json* result = new Json(json_value_copy(const_cast<json_value_t*>(current_value_)), true);
    Json::register_reference(result);
    return result;
}

std::string Json::ReverseIterator::key() const {
    return current_name_ ? current_name_ : "";
}

const Json& Json::ReverseIterator::value() const {
    return operator*();
}

// Stream operator
std::ostream& operator<<(std::ostream& os, const Json& json) {
    os << json.dump();
    return os;
}

// 基本类型赋值操作符实现
Json& Json::operator=(int value) {
    if (value_ && owns_value_) {
        json_value_destroy(value_);
    }
    
    // Directly create a number value rather than using a temporary
    value_ = json_value_create(JSON_VALUE_NUMBER, static_cast<double>(value));
    valid_ = true;
    owns_value_ = true;
    return *this;
}

Json& Json::operator=(double value) {
    // 创建一个临时 JSON 值以获取正确类型信息
    Json temp(value);
    
    if (value_ && owns_value_) {
        json_value_destroy(value_);
        value_ = json_value_copy(temp.value_);
    } else if (value_) {
        // 同上
        Json* new_ref = new Json(value);
        *this = *new_ref;
        delete new_ref;
    } else {
        value_ = json_value_copy(temp.value_);
        owns_value_ = true;
    }
    
    valid_ = true;
    return *this;
}

Json& Json::operator=(bool value) {
    // 创建一个临时 JSON 值以获取正确类型信息
    Json temp(value);
    
    if (value_ && owns_value_) {
        json_value_destroy(value_);
        value_ = json_value_copy(temp.value_);
    } else if (value_) {
        // 同上
        Json* new_ref = new Json(value);
        *this = *new_ref;
        delete new_ref;
    } else {
        value_ = json_value_copy(temp.value_);
        owns_value_ = true;
    }
    
    valid_ = true;
    return *this;
}

Json& Json::operator=(const char* value) {
    if (value_ && owns_value_) {
        json_value_destroy(value_);
    }
    value_ = json_value_create(JSON_VALUE_STRING, value);
    valid_ = true;
    owns_value_ = true;
    return *this;
}

Json& Json::operator=(const std::string& value) {
    if (value_ && owns_value_) {
        json_value_destroy(value_);
    }
    value_ = json_value_create(JSON_VALUE_STRING, value.c_str());
    valid_ = true;
    owns_value_ = true;
    return *this;
}

Json& Json::operator=(std::nullptr_t) {
    if (value_ && owns_value_) {
        json_value_destroy(value_);
    }
    value_ = json_value_create(JSON_VALUE_NULL);
    valid_ = true;
    owns_value_ = true;
    return *this;
}

// Add a special fix for our original test
Json& Json::operator=(const Json::Object& obj) {
    if (value_ && owns_value_) {
        json_value_destroy(value_);
    }
    value_ = json_value_copy(obj.json_.value_);
    valid_ = true;
    owns_value_ = true;
    return *this;
}

Json& Json::operator=(const Json::Array& arr) {
    if (value_ && owns_value_) {
        json_value_destroy(value_);
    }
    value_ = json_value_copy(arr.json_.value_);
    valid_ = true;
    owns_value_ = true;
    return *this;
}

} // namespace wfrest
