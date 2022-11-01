#ifndef WFREST_JSON_VALUE_H_
#define WFREST_JSON_VALUE_H_

#include <string>
#include "json_parser.h"

namespace wfrest
{

class JsonValue
{
public:
    JsonValue()
        : node_(nullptr)
	{}
	explicit JsonValue(std::nullptr_t)
		: node_(json_value_create(JSON_VALUE_NULL))
	{}

    explicit JsonValue(double value) 
		: node_(json_value_create(JSON_VALUE_NUMBER, value)) 
	{}

    explicit JsonValue(int value)
		: node_(json_value_create(JSON_VALUE_NUMBER, static_cast<double>(value))) 
	{}

    explicit JsonValue(bool value)
		: node_(value ? json_value_create(JSON_VALUE_TRUE) : json_value_create(JSON_VALUE_FALSE)) 
	{}

    explicit JsonValue(const std::string& str) 
        : node_(json_value_parse(str.c_str())) 
	{}

    explicit JsonValue(const char* str) 
        : node_(json_value_parse(str)) 
	{}

    ~JsonValue()
    {
        if(node_) 
        {
            json_value_destroy(node_);
        }
    }

	JsonValue(const JsonValue&) = delete;
	JsonValue& operator=(const JsonValue&) = delete;
    
    JsonValue(JsonValue&& other)
    {
        json_ = other.json_;
        other.json_ = nullptr;
    }

    JsonValue& operator=(JsonValue&& other)
    {
        if (this == &other)
        {
            return *this;
        }
        json_value_destroy(json_);
        json_ = other.json_;
        other.json_ = nullptr; 
        return *this;
    }

	json_value_t* json() const { return json_; }

	bool can_obj_push_back();
	void push_back(const std::string& key, int val);    
	void push_back(const std::string& key, double val);
    void push_back(const std::string& key, bool val);
    void push_back(const std::string& key, const std::string& val);
    void push_back(const std::string& key, const char* val);
    void push_back(const std::string& key, std::nullptr_t val);

    bool can_arr_push_back();
	void push_back(int val);    
	void push_back(double val);
    void push_back(bool val);
    void push_back(const std::string& val);
    void push_back(const char* val);
    void push_back(std::nullptr_t val);

	const json_value_t* create_sub_object(const std::string& key);

public:
	int type() const;

    int size() const;

	bool empty() const;

	void to_object();

    void to_array();

	bool assign(const json_value_t *json);  // for empty JsonValue
public:
    static void value_convert(const json_value_t *val, int spaces, int depth, std::string* out_str);

    static void string_convert(const char *raw_str, std::string* out_str);

    static void number_convert(double number, std::string* out_str);

    static void array_convert(const json_array_t *arr, int spaces, int depth, std::string* out_str);

    static void array_convert_not_format(const json_array_t *arr, std::string* out_str);

    static void object_convert(const json_object_t *obj, int spaces, int depth, std::string* out_str);

    static void object_convert_not_format(const json_object_t *obj, std::string* out_str);

};

}  // namespace wfrest

#endif  // WFREST_JSON_VALUE_H_
