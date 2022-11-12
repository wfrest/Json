#ifndef WFREST_JSON_H_
#define WFREST_JSON_H_

#include "json_parser.h"
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace wfrest
{

namespace detail
{

template <typename T>
struct is_string
{
    static constexpr bool value = false;
};

template <class T, class Traits, class Alloc>
struct is_string<std::basic_string<T, Traits, Alloc>>
{
    static constexpr bool value = true;
};

template <typename C>
struct is_char
    : std::integral_constant<bool, std::is_same<C, char>::value ||
                                       std::is_same<C, char16_t>::value ||
                                       std::is_same<C, char32_t>::value ||
                                       std::is_same<C, wchar_t>::value>
{
};

template <typename T>
struct is_number
    : std::integral_constant<bool, std::is_arithmetic<T>::value &&
                                       !std::is_same<T, bool>::value &&
                                       !detail::is_char<T>::value>
{
};

} // namespace detail

class Object_S;
class Array_S;
class Json
{
public:
    using Object = Object_S;
    using Array = Array_S;

public:
    static Json parse(const std::string& str);
    static Json parse(const std::ifstream& stream);

    const std::string dump() const;
    const std::string dump(int spaces) const;

    Json operator[](const std::string& key);

    Json operator[](const std::string& key) const;

    Json operator[](int index);

    Json operator[](int index) const;

    template <typename T>
    void operator=(const T& val)
    {
        this->push_back(parent_key_, val);
    }

    bool has(const std::string& key) const;

    template <typename T>
    typename std::enable_if<std::is_same<T, bool>::value, T>::type get() const
    {
        return json_value_type(node_) == JSON_VALUE_TRUE ? true : false;
    }

    template <typename T>
    typename std::enable_if<detail::is_number<T>::value, T>::type get() const
    {
        return static_cast<T>(json_value_number(node_));
    }

    template <typename T>
    typename std::enable_if<detail::is_string<T>::value, T>::type get() const
    {
        return std::string(json_value_string(node_));
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, Object>::value, T>::type
    get() const;

    template <typename T>
    typename std::enable_if<std::is_same<T, Array>::value, T>::type get() const;

    template <typename T>
    typename std::enable_if<std::is_same<T, std::nullptr_t>::value, T>::type
    get() const
    {
        return nullptr;
    }

    template <typename T>
    operator T()
    {
        return get<T>();
    }

public:
    int type() const
    {
        return json_value_type(node_);
    }

    std::string type_str() const;

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

    bool empty() const;

    void clear();

public:
    // for object
    template <typename T>
    void push_back(const std::string& key, const T& val)
    {
        if (!can_obj_push_back())
        {
            return;
        }
        if (is_placeholder())
        {
            placeholder_push_back(key, val);
        }
        else
        {
            normal_push_back(key, val);
        }
    }

    template <typename T, typename std::enable_if<detail::is_number<T>::value,
                                                  bool>::type = true>
    void placeholder_push_back(const std::string& key, const T& val)
    {
        json_object_t* obj = json_value_object(parent_);
        destroy_node(node_);
        node_ = json_object_append(obj, key.c_str(), JSON_VALUE_NUMBER,
                                   static_cast<double>(val));
    }

    void placeholder_push_back(const std::string& key, bool val)
    {
        json_object_t* obj = json_value_object(parent_);
        destroy_node(node_);
        if (val)
        {
            node_ = json_object_append(obj, key.c_str(), JSON_VALUE_TRUE);
        }
        else
        {
            node_ = json_object_append(obj, key.c_str(), JSON_VALUE_FALSE);
        }
    }

    void placeholder_push_back(const std::string& key, std::nullptr_t val)
    {
        json_object_t* obj = json_value_object(parent_);
        destroy_node(node_);
        node_ = json_object_append(obj, key.c_str(), JSON_VALUE_NULL);
    }

    void placeholder_push_back(const std::string& key, const std::string& val)
    {
        placeholder_push_back(key, val.c_str());
    }

    void placeholder_push_back(const std::string& key, const char* val)
    {
        json_object_t* obj = json_value_object(parent_);
        destroy_node(node_);
        node_ = json_object_append(obj, key.c_str(), JSON_VALUE_STRING, val);
    }
    template <typename T,
              typename std::enable_if<std::is_same<T, Object>::value ||
                                          std::is_same<T, Array>::value,
                                      bool>::type = true>
    void placeholder_push_back(const std::string& key, const T& val);

    template <typename T, typename std::enable_if<detail::is_number<T>::value,
                                                  bool>::type = true>
    void normal_push_back(const std::string& key, const T& val)
    {
        json_object_t* obj = json_value_object(node_);
        json_object_append(obj, key.c_str(), JSON_VALUE_NUMBER,
                           static_cast<double>(val));
    }

    void normal_push_back(const std::string& key, bool val)
    {
        json_object_t* obj = json_value_object(node_);
        if (val)
        {
            json_object_append(obj, key.c_str(), JSON_VALUE_TRUE);
        }
        else
        {
            json_object_append(obj, key.c_str(), JSON_VALUE_FALSE);
        }
    }

    void normal_push_back(const std::string& key, std::nullptr_t val)
    {
        json_object_t* obj = json_value_object(node_);
        json_object_append(obj, key.c_str(), JSON_VALUE_NULL);
    }

    void normal_push_back(const std::string& key, const std::string& val)
    {
        normal_push_back(key, val.c_str());
    }

    void normal_push_back(const std::string& key, const char* val)
    {
        json_object_t* obj = json_value_object(node_);
        json_object_append(obj, key.c_str(), JSON_VALUE_STRING, val);
    }

    template <typename T,
              typename std::enable_if<std::is_same<T, Object>::value ||
                                          std::is_same<T, Array>::value,
                                      bool>::type = true>
    void normal_push_back(const std::string& key, const T& val);

    // for array
    void push_back(int val);
    void push_back(double val);
    void push_back(bool val);
    void push_back(const std::string& val);
    void push_back(const char* val);
    void push_back(std::nullptr_t val);
    void push_back(const Object& obj);

private:
    bool can_obj_push_back();

    bool can_arr_push_back();

    void to_object();

    void to_array();

    // can convert to any type, just like a placeholder
    bool is_placeholder() const
    {
        return is_null() && parent_ != nullptr;
    }

    bool is_incomplete() const
    {
        return node_ == nullptr;
    }

    void destroy_node(const json_value_t* node)
    {
        if (node != nullptr)
        {
            json_value_destroy(const_cast<json_value_t*>(node));
        }
    }

    Json copy() const
    {
        // todo : need to optimize
        return Json(dump());
    }

private:
    // for parse
    static void value_convert(const json_value_t* val, int spaces, int depth,
                              std::string* out_str);

    static void string_convert(const char* raw_str, std::string* out_str);

    static void number_convert(double number, std::string* out_str);

    static void array_convert(const json_array_t* arr, int spaces, int depth,
                              std::string* out_str);

    static void array_convert_not_format(const json_array_t* arr,
                                         std::string* out_str);

    static void object_convert(const json_object_t* obj, int spaces, int depth,
                               std::string* out_str);

    static void object_convert_not_format(const json_object_t* obj,
                                          std::string* out_str);

    friend inline std::ostream& operator<<(std::ostream& os, const Json& json)
    {
        return (os << json.dump());
    }

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

    Json(const Json& json) = delete;
    Json& operator=(const Json& json) = delete;
    Json(Json&& other);
    Json& operator=(Json&& other);

protected:
    struct Empty
    {
    };
    // watcher
    Json(const json_value_t* parent, std::string&& key);
    Json(const json_value_t* parent, const std::string& key);
    Json(const json_value_t* parent);
    Json(const json_value_t* node, const json_value_t* parent);
    Json(const Empty&);

    bool is_valid() const
    {
        return node_ != nullptr;
    }

    bool is_root() const
    {
        return parent_ == nullptr;
    }

private:
    const json_value_t* node_ = nullptr;
    const json_value_t* parent_ = nullptr;
    std::string parent_key_;
};

class Object_S : public Json
{
public:
    Object_S() : Json(*this)
    {
    }
    Object_S(const json_value_t* node, const json_value_t* parent)
        : Json(node, parent)
    {
    }
};

class Array_S : public Json
{
public:
    Array_S() : Json(*this)
    {
    }
    Array_S(const json_value_t* node, const json_value_t* parent)
        : Json(node, parent)
    {
    }
};

template <typename T>
typename std::enable_if<std::is_same<T, Json::Object>::value, T>::type
Json::get() const
{
    return Json::Object(node_, parent_);
}

template <typename T>
typename std::enable_if<std::is_same<T, Json::Array>::value, T>::type
Json::get() const
{
    return Json::Array(node_, parent_);
}

template <typename T,
          typename std::enable_if<std::is_same<T, Json::Object>::value ||
                                      std::is_same<T, Json::Array>::value,
                                  bool>::type = true>
void Json::placeholder_push_back(const std::string& key, const T& val)
{
    json_object_t* obj = json_value_object(parent_);
    destroy_node(node_);
    Json copy_json = val.copy();
    node_ = json_object_append(obj, key.c_str(), 0, copy_json.node_);
    copy_json.node_ = nullptr;
}

template <typename T,
          typename std::enable_if<std::is_same<T, Json::Object>::value ||
                                      std::is_same<T, Json::Array>::value,
                                  bool>::type = true>
void Json::normal_push_back(const std::string& key, const T& val)
{
    json_object_t* obj = json_value_object(node_);
    Json copy_json = val.copy();
    json_object_append(obj, key.c_str(), 0, copy_json.node_);
    copy_json.node_ = nullptr;
}

} // namespace wfrest

#endif // WFREST_JSON_H_
