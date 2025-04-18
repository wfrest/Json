#ifndef JSON_WRAPPER_HPP
#define JSON_WRAPPER_HPP

#include <stdexcept>
#include <string>
#include <sstream>
#include <memory>
#include <iterator>
#include <map>
#include <vector>
#include "json_parser.h"

namespace wfrest {

// Forward declarations
class JsonObject;
class JsonArray;

class Json {
private:
    using JsonValuePtr = std::shared_ptr<json_value_t>;

    JsonValuePtr value;
    mutable std::map<std::string, Json> object_cache;
    mutable std::vector<Json> array_cache;

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

    // Proxy class for assignment
    class Proxy {
    private:
        Json& parent;
        std::string key;  // For objects
        size_t index;     // For arrays
        bool is_array_p;

        // Get a reference to the actual Json object
        Json& get_json() {
            if (is_array_p) {
                return parent.at_internal(index);
            } else {
                return parent.at_internal(key);
            }
        }

        // Get a const reference to the actual Json object
        const Json& get_json() const {
            if (is_array_p) {
                return parent.at_internal(index);
            } else {
                return parent.at_internal(key);
            }
        }

    public:
        Proxy(Json& p, const std::string& k) : parent(p), key(k), index(0), is_array_p(false) {}
        Proxy(Json& p, size_t i) : parent(p), key(""), index(i), is_array_p(true) {}

        // Allow Proxy to be treated as a Json& for chaining
        operator Json&() {
            return get_json();
        }

        // Allow access to nested elements
        Proxy operator[](const std::string& k) {
            Json& ref = get_json();
            return ref[k];
        }

        Proxy operator[](size_t i) {
            Json& ref = get_json();
            return ref[i];
        }

        // Forward type-checking methods
        int type() const { return get_json().type(); }
        std::string type_str() const { return get_json().type_str(); }
        bool is_object() const { return get_json().is_object(); }
        bool is_array() const { return get_json().is_array(); }
        bool is_string() const { return get_json().is_string(); }
        bool is_number() const { return get_json().is_number(); }
        bool is_boolean() const { return get_json().is_boolean(); }
        bool is_null() const { return get_json().is_null(); }

        // Forward value access methods
        template<typename T>
        T get() const { return get_json().get<T>(); }

        // Forward utility methods
        std::string dump() const { return get_json().dump(); }
        size_t size() const { return get_json().size(); }
        bool has(const std::string& k) const { return get_json().has(k); }
        void push_back(const Json& item) { get_json().push_back(item); }
        void push_back(const std::string& k, const Json& item) { get_json().push_back(k, item); }

        Proxy& operator=(const Json& other) {
            if (is_array_p) {
                // For array elements, replace the value directly in the array_cache
                if (index < parent.array_cache.size()) {
                    parent.array_cache[index] = other;
                    
                    // Also update the underlying array in the JSON value
                    json_array_t* arr = json_value_array(parent.value.get());
                    
                    // Get the current value at this index to replace it
                    if (arr) {
                        // First remove existing element if any
                        size_t current_size = json_array_size(arr);
                        if (index < current_size) {
                            size_t i = 0;
                            const json_value_t* v = nullptr;
                            const json_value_t* target = nullptr;
                            json_array_for_each(v, arr) {
                                if (i == index) {
                                    target = v;
                                    break;
                                }
                                i++;
                            }
                            
                            if (target) {
                                json_array_remove(target, arr);
                                
                                // Now insert the new value at the same position
                                json_value_t* copy = json_value_copy(other.value.get());
                                int type = json_value_type(copy);
                                
                                // We need to insert it at the right position
                                json_value_t* new_value = nullptr;
                                
                                switch (type) {
                                    case JSON_VALUE_STRING:
                                        new_value = json_value_create(type, json_value_string(copy));
                                        break;
                                    case JSON_VALUE_NUMBER:
                                        new_value = json_value_create(type, json_value_number(copy));
                                        break;
                                    case JSON_VALUE_OBJECT:
                                    case JSON_VALUE_ARRAY:
                                        // For complex types, we need to do a deep copy
                                        new_value = json_value_copy(copy);
                                        break;
                                    default:
                                        new_value = json_value_create(type);
                                        break;
                                }
                                
                                // Insert at position
                                if (new_value) {
                                    size_t pos = 0;
                                    json_array_t* new_arr = json_value_array(parent.value.get());
                                    
                                    // Get to the index position
                                    json_value_t* insert_pos = nullptr;
                                    if (index > 0) {
                                        size_t i = 0;
                                        const json_value_t* v = nullptr;
                                        json_array_for_each(v, new_arr) {
                                            if (i == index - 1) {
                                                insert_pos = (json_value_t*)v;
                                                break;
                                            }
                                            i++;
                                        }
                                    }
                                    
                                    // Insert after the found position or at beginning
                                    if (insert_pos) {
                                        json_array_insert_after(insert_pos, new_arr, json_value_type(new_value), new_value);
                                    } else {
                                        // Insert at the beginning
                                        json_array_append(new_arr, json_value_type(new_value), new_value);
                                    }
                                    
                                    json_value_destroy(new_value);
                                }
                            }
                        }
                    }
                }
            } else {
                // For object properties, use the same approach as before
                json_object_t* obj = json_value_object(parent.value.get());
                const json_value_t* old_val = json_object_find(key.c_str(), obj);
                if (old_val) {
                    json_object_remove(old_val, obj);
                }
                
                json_value_t* copy = json_value_copy(other.value.get());
                int val_type = json_value_type(copy);
                
                switch (val_type) {
                    case JSON_VALUE_STRING:
                        json_object_append(obj, key.c_str(), val_type, json_value_string(copy));
                        break;
                    case JSON_VALUE_NUMBER:
                        json_object_append(obj, key.c_str(), val_type, json_value_number(copy));
                        break;
                    case JSON_VALUE_OBJECT: {
                        // Deep copy the object
                        json_object_t* copy_obj = json_value_object(copy);
                        json_object_append(obj, key.c_str(), JSON_VALUE_OBJECT, copy_obj);
                        break;
                    }
                    case JSON_VALUE_ARRAY: {
                        // Deep copy the array
                        json_array_t* copy_arr = json_value_array(copy);
                        json_object_append(obj, key.c_str(), JSON_VALUE_ARRAY, copy_arr);
                        break;
                    }
                    default:
                        json_object_append(obj, key.c_str(), val_type);
                        break;
                }
                
                json_value_destroy(copy);
                
                // Update the cache
                parent.object_cache[key] = other;
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

    // Helper methods to get elements
    Json get_element(const std::string& key) const {
        if (!is_object()) {
            throw std::runtime_error("Not an object");
        }
        json_object_t* obj = json_value_object(value.get());
        const json_value_t* val = json_object_find(key.c_str(), obj);
        if (!val) {
            val = json_object_append(obj, key.c_str(), JSON_VALUE_NULL);
        }
        
        // Create a new JSON value by copying the element
        json_value_t* copy = json_value_copy(val);
        return Json(JsonValuePtr(copy, json_value_destroy));
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
        
        // Create a new JSON value by copying the element
        json_value_t* copy = json_value_copy(val);
        return Json(JsonValuePtr(copy, json_value_destroy));
    }

    // Internal methods for reference access
    Json& at_internal(const std::string& key) const {
        if (!is_object()) {
            const_cast<Json*>(this)->value.reset(json_value_create(JSON_VALUE_OBJECT), json_value_destroy);
            const_cast<Json*>(this)->object_cache.clear();
        }
        
        auto it = object_cache.find(key);
        if (it == object_cache.end()) {
            // Create or get the key
            json_object_t* obj = json_value_object(value.get());
            const json_value_t* val = json_object_find(key.c_str(), obj);
            if (!val) {
                val = json_object_append(obj, key.c_str(), JSON_VALUE_NULL);
            }
            
            // Create a new cache entry
            json_value_t* copy = json_value_copy(val);
            Json cached_value(JsonValuePtr(copy, json_value_destroy));
            auto result = object_cache.emplace(key, std::move(cached_value));
            return result.first->second;
        }
        
        return it->second;
    }

    Json& at_internal(size_t index) const {
        if (!is_array()) {
            const_cast<Json*>(this)->value.reset(json_value_create(JSON_VALUE_ARRAY), json_value_destroy);
            const_cast<Json*>(this)->array_cache.clear();
        }
        
        json_array_t* arr = json_value_array(value.get());
        
        // Make sure the array is big enough
        while (array_cache.size() <= index) {
            // Create a null value
            json_value_t* null = json_value_create(JSON_VALUE_NULL);
            json_array_append(arr, JSON_VALUE_NULL);
            const_cast<Json*>(this)->array_cache.emplace_back(JsonValuePtr(json_value_copy(null), json_value_destroy));
            json_value_destroy(null);
        }
        
        return array_cache[index];
    }

public:
    // Aliases for standard collections
    using Array = JsonArray;
    using Object = JsonObject;
    
    // Basic constructors
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

    // Constructor for shared_ptr (needed for internal cache)
    Json(JsonValuePtr ptr) : value(ptr) {}

    // Copy and move
    Json(const Json& other) : value(other.value) {}
    Json(Json&& other) noexcept : value(std::move(other.value)), 
                                 object_cache(std::move(other.object_cache)),
                                 array_cache(std::move(other.array_cache)) {}

    Json& operator=(const Json& other) {
        if (this != &other) {
            value = other.value;
            object_cache.clear();
            array_cache.clear();
        }
        return *this;
    }

    // Management methods
    void clear() {
        if (is_object()) {
            value.reset(json_value_create(JSON_VALUE_OBJECT), json_value_destroy);
        } else if (is_array()) {
            value.reset(json_value_create(JSON_VALUE_ARRAY), json_value_destroy);
        }
        object_cache.clear();
        array_cache.clear();
    }
    
    bool empty() const {
        return size() == 0;
    }
    
    void erase(const std::string& key) {
        if (!is_object()) return;
        
        json_object_t* obj = json_value_object(value.get());
        const json_value_t* val = json_object_find(key.c_str(), obj);
        if (val) {
            json_object_remove(val, obj);
            object_cache.erase(key);
        }
    }

    // Factory methods
    static Json create_array();
    static Json create_object();

    // Type methods
    int type() const {
        if (!value)
            return JSON_VALUE_NULL;
        return json_value_type(value.get());
    }

    std::string type_str() const {
        switch (type()) {
            case JSON_VALUE_NULL:   return "null";
            case JSON_VALUE_TRUE:   return "true";
            case JSON_VALUE_FALSE:  return "false";
            case JSON_VALUE_NUMBER: return "number";
            case JSON_VALUE_STRING: return "string";
            case JSON_VALUE_ARRAY:  return "array";
            case JSON_VALUE_OBJECT: return "object";
            default:                return "unknown";
        }
    }

    // Type checking
    bool is_object() const { return json_value_type(value.get()) == JSON_VALUE_OBJECT; }
    bool is_array() const { return json_value_type(value.get()) == JSON_VALUE_ARRAY; }
    bool is_string() const { return json_value_type(value.get()) == JSON_VALUE_STRING; }
    bool is_number() const { return json_value_type(value.get()) == JSON_VALUE_NUMBER; }
    bool is_boolean() const { return json_value_type(value.get()) == JSON_VALUE_TRUE || 
                                   json_value_type(value.get()) == JSON_VALUE_FALSE; }
    bool is_null() const { return json_value_type(value.get()) == JSON_VALUE_NULL; }

    // Assigning and reading
    Proxy operator[](const std::string& key) { return Proxy(*this, key); }
    Proxy operator[](size_t index) { return Proxy(*this, index); }

    // Reading
    Json at(const std::string& key) const { return get_element(key); }
    Json at(size_t index) const { return get_element(index); }

    // Value getters
    template<typename T>
    T get() const;

    // Add elements to collections
    void push_back(const Json& item);
    void push_back(const std::string& key, const Json& item);

    // Collection size
    size_t size() const {
        if (is_array()) return json_array_size(json_value_array(value.get()));
        if (is_object()) return json_object_size(json_value_object(value.get()));
        return 0;
    }

    // Check for key in object
    bool has(const std::string& key) const {
        if (!is_object()) return false;
        return json_object_find(key.c_str(), json_value_object(value.get())) != nullptr;
    }

    // Serialization
    std::string dump() const {
        return dump_value(value.get());
    }

    // Parsing
    static Json parse(const std::string& str) {
        return Json(str);
    }

    // For internal use by iterators
    friend Json createJsonFromInternalValue(const json_value_t* val);
    friend JsonValuePtr getValuePtr(const Json& json);
    
    friend class Proxy;
    friend class JsonArray;
    friend class JsonObject;
};

// Helper function to create Json from internal value
inline Json createJsonFromInternalValue(const json_value_t* val) {
    if (!val) return Json(nullptr);
    
    json_value_t* copy = json_value_copy(val);
    return Json(Json::JsonValuePtr(copy, json_value_destroy));
}

// Helper function to get value ptr from Json
inline Json::JsonValuePtr getValuePtr(const Json& json) {
    return json.value;
}

// Template specializations for get()
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

// Forward declare for use in JsonArray and JsonObject
class JsonArray;
class JsonObject;

// Implementation of factory methods
inline Json Json::create_array() {
    json_value_t* arr = json_value_create(JSON_VALUE_ARRAY);
    return Json(JsonValuePtr(arr, json_value_destroy));
}

inline Json Json::create_object() {
    json_value_t* obj = json_value_create(JSON_VALUE_OBJECT);
    return Json(JsonValuePtr(obj, json_value_destroy));
}

// JsonArray implementation
class JsonArray : public Json {
public:
    JsonArray() {
        value.reset(json_value_create(JSON_VALUE_ARRAY), json_value_destroy);
    }
    
    // Add initializer list constructor for arrays
    JsonArray(std::initializer_list<Json> items) {
        value.reset(json_value_create(JSON_VALUE_ARRAY), json_value_destroy);
        for (const auto& item : items) {
            push_back(item);
        }
    }
};

// JsonObject implementation
class JsonObject : public Json {
public:
    JsonObject() {
        // The default Json constructor already creates an object
    }
    
    // Add initializer list constructor for objects
    JsonObject(std::initializer_list<std::pair<std::string, Json>> items) {
        value.reset(json_value_create(JSON_VALUE_OBJECT), json_value_destroy);
        for (const auto& pair : items) {
            push_back(pair.first, pair.second);
        }
    }
};

// Iterator simplified to not store Json objects directly
class JsonIterator {
private:
    const Json* parent;
    bool is_object;
    bool is_end;
    size_t index;
    const char* current_key;
    const json_value_t* current_value;

public:
    // Default constructor for end iterator
    JsonIterator() : parent(nullptr), is_object(false), is_end(true), index(0), 
                  current_key(nullptr), current_value(nullptr) {}

    // Constructor for begin iterator
    JsonIterator(const Json* json, bool end = false) 
        : parent(json), is_object(json->is_object()), is_end(end), index(0),
          current_key(nullptr), current_value(nullptr) {
        if (!parent || is_end) return;

        if (is_object) {
            json_object_t* obj = json_value_object(getValuePtr(*parent).get());
            current_key = json_object_next_name(nullptr, obj);
            current_value = json_object_next_value(nullptr, obj);
            is_end = (current_value == nullptr);
        } else if (parent->is_array()) {
            json_array_t* arr = json_value_array(getValuePtr(*parent).get());
            current_value = json_array_next_value(nullptr, arr);
            is_end = (current_value == nullptr);
        } else {
            is_end = true;
        }
    }

    // Pre-increment operator
    JsonIterator& operator++() {
        if (is_end) return *this;

        if (is_object) {
            json_object_t* obj = json_value_object(getValuePtr(*parent).get());
            current_key = json_object_next_name(current_key, obj);
            current_value = json_object_next_value(current_value, obj);
            is_end = (current_value == nullptr);
        } else {
            json_array_t* arr = json_value_array(getValuePtr(*parent).get());
            current_value = json_array_next_value(current_value, arr);
            index++;
            is_end = (current_value == nullptr);
        }
        return *this;
    }

    // Post-increment operator
    JsonIterator operator++(int) {
        JsonIterator tmp(*this);
        ++(*this);
        return tmp;
    }

    // Equality operators
    bool operator==(const JsonIterator& other) const {
        if (is_end && other.is_end) return true;
        return parent == other.parent && current_value == other.current_value;
    }

    bool operator!=(const JsonIterator& other) const {
        return !(*this == other);
    }

    // Access methods
    std::string key() const {
        return current_key ? std::string(current_key) : "";
    }

    Json value() const {
        return createJsonFromInternalValue(current_value);
    }

    // Dereference operator
    Json operator*() const {
        return value();
    }
};

// Implementation of push_back methods
inline void Json::push_back(const Json& item) {
    if (!is_array()) {
        value.reset(json_value_create(JSON_VALUE_ARRAY), json_value_destroy);
        array_cache.clear();
    }
    
    json_array_t* arr = json_value_array(value.get());
    
    json_value_t* copy = json_value_copy(item.value.get());
    int val_type = json_value_type(copy);

    switch (val_type) {
        case JSON_VALUE_STRING:
            json_array_append(arr, JSON_VALUE_STRING, json_value_string(copy));
            break;
        case JSON_VALUE_NUMBER:
            json_array_append(arr, JSON_VALUE_NUMBER, json_value_number(copy));
            break;
        case JSON_VALUE_OBJECT:
            // Deep copy the entire object
            json_array_append(arr, JSON_VALUE_OBJECT, json_value_object(copy));
            break;
        case JSON_VALUE_ARRAY:
            // Deep copy the array
            json_array_append(arr, JSON_VALUE_ARRAY, json_value_array(copy));
            break;
        case JSON_VALUE_TRUE:
            json_array_append(arr, JSON_VALUE_TRUE);
            break;
        case JSON_VALUE_FALSE:
            json_array_append(arr, JSON_VALUE_FALSE);
            break;
        case JSON_VALUE_NULL:
        default:
            json_array_append(arr, JSON_VALUE_NULL);
            break;
    }
    
    json_value_destroy(copy);
    
    // Update cache
    json_value_t* new_copy = json_value_copy(item.value.get());
    array_cache.emplace_back(Json::JsonValuePtr(new_copy, json_value_destroy));
}

inline void Json::push_back(const std::string& key, const Json& item) {
    if (!is_object()) {
        value.reset(json_value_create(JSON_VALUE_OBJECT), json_value_destroy);
        object_cache.clear();
    }
    
    json_object_t* obj = json_value_object(value.get());
    
    json_value_t* copy = json_value_copy(item.value.get());
    int val_type = json_value_type(copy);

    switch (val_type) {
        case JSON_VALUE_STRING:
            json_object_append(obj, key.c_str(), JSON_VALUE_STRING, json_value_string(copy));
            break;
        case JSON_VALUE_NUMBER:
            json_object_append(obj, key.c_str(), JSON_VALUE_NUMBER, json_value_number(copy));
            break;
        case JSON_VALUE_OBJECT:
            // Deep copy the object
            json_object_append(obj, key.c_str(), JSON_VALUE_OBJECT, json_value_object(copy));
            break;
        case JSON_VALUE_ARRAY:
            // Deep copy the array
            json_object_append(obj, key.c_str(), JSON_VALUE_ARRAY, json_value_array(copy));
            break;
        case JSON_VALUE_TRUE:
            json_object_append(obj, key.c_str(), JSON_VALUE_TRUE);
            break;
        case JSON_VALUE_FALSE:
            json_object_append(obj, key.c_str(), JSON_VALUE_FALSE);
            break;
        case JSON_VALUE_NULL:
        default:
            json_object_append(obj, key.c_str(), JSON_VALUE_NULL);
            break;
    }
    
    json_value_destroy(copy);
    
    // Update cache
    json_value_t* new_copy = json_value_copy(item.value.get());
    object_cache[key] = Json(Json::JsonValuePtr(new_copy, json_value_destroy));
}

// Add iterator typedefs
typedef JsonIterator iterator;

// Implement begin and end functions
inline iterator begin(const Json& json) {
    return iterator(&json);
}

inline iterator end(const Json& json) {
    return iterator(&json, true);
}

} // namespace wfrest

#endif
