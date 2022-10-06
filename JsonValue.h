#ifndef WFREST_JSON_VALUE_H_
#define WFREST_JSON_VALUE_H_

#include "Json.h"

namespace wfrest
{

// inner class
class JsonValue
{
public:
	friend Json;

	explicit JsonValue(std::nullptr_t)
		: json_(json_value_create(JSON_VALUE_NULL)) 
	{}

    explicit JsonValue(double value) 
		: json_(json_value_create(JSON_VALUE_NUMBER, value)) 
	{}

    explicit JsonValue(int value)
		: json_(json_value_create(JSON_VALUE_NUMBER, static_cast<double>(value))) 
	{}

    explicit JsonValue(bool value)
		: json_(value ? json_value_create(JSON_VALUE_TRUE) : json_value_create(JSON_VALUE_FALSE)) 
	{}

    explicit JsonValue(const std::string& str) 
        : json_(json_value_parse(str.c_str())) 
	{}

    explicit JsonValue(const char* str) 
        : json_(json_value_parse(str)) 
	{}

    explicit JsonValue(const Json::Object& obj) 
        : json_(json_value_create(JSON_VALUE_OBJECT)) 
	{}

	explicit JsonValue(const json_value_t* val)
		: allocate_(false), 
		json_(const_cast<json_value_t *>(val))
	{}

    ~JsonValue()
    {
        if(json_ && allocate_) 
        {
            json_value_destroy(json_);
        }
    }

	JsonValue(const JsonValue&) = delete;
	JsonValue& operator=(const JsonValue&) = delete;
    JsonValue(JsonValue&& other) = delete;
    JsonValue& operator=(JsonValue&& other) = delete;

    void operator=(const std::string& str);

	json_value_t* json() { return json_; }

	bool can_push_back();
	void push_back(const std::string& key, int val);    
	void push_back(const std::string& key, double val);
    void push_back(const std::string& key, bool val);
    void push_back(const std::string& key, const std::string& val);
    void push_back(const std::string& key, const char* val);
    void push_back(const std::string& key, std::nullptr_t val);

	const json_value_t* create_sub_object(const std::string& key);

public:
	// todo : need optimize in modern way
	int type() const;

	void to_object();

	bool empty() const;

	bool assign(const json_value_t *json);
public:
    static void value_convert(const json_value_t *val, int spaces, int depth, std::string* out_str);

    static void string_convert(const char *raw_str, std::string* out_str);

    static void number_convert(double number, std::string* out_str);

    static void array_convert(const json_array_t *arr, int spaces, int depth, std::string* out_str);

    static void array_convert_not_format(const json_array_t *arr, std::string* out_str);

    static void object_convert(const json_object_t *obj, int spaces, int depth, std::string* out_str);

    static void object_convert_not_format(const json_object_t *obj, std::string* out_str);

private:
    JsonValue() 
        : json_(nullptr) 
	{}

private:
	bool allocate_ = true;
    std::string key_;
    json_value_t *json_ = nullptr;
};

}  // namespace wfrest

#endif  // WFREST_JSON_VALUE_H_
