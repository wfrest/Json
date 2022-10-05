#include "Json.h"
#include <cassert>

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

	void push_back(const std::string& key, int val);    
	void push_back(const std::string& key, double val);
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

int JsonValue::type() const
{
	return json_value_type(json_);
}

bool JsonValue::empty() const
{
	return json_ == nullptr;
}

void JsonValue::push_back(const std::string& key, int val)
{
	this->push_back(key, static_cast<double>(val));
}

void JsonValue::push_back(const std::string& key, double val)
{
	if(this->type() == JSON_VALUE_NULL)
	{
		this->to_object();
	}
	else if(this->type() != JSON_VALUE_OBJECT)
	{
		return;
	}
	json_object_t* obj = json_value_object(json_);
	json_object_append(obj, key.c_str(), JSON_VALUE_NUMBER, val);
}

void JsonValue::to_object()
{
	// TODO : optimize
	json_value_destroy(json_);
	json_ = json_value_create(JSON_VALUE_OBJECT);
}

const json_value_t* JsonValue::create_sub_object(const std::string& key)
{
	json_object_t *obj = json_value_object(json_);
	const json_value_t* sub_obj = json_object_append(obj, key.c_str(), JSON_VALUE_OBJECT);
	return sub_obj;
}

bool JsonValue::assign(const json_value_t *json)
{
	if(json_ != nullptr)
	{
		return false;
	}
	json_ = const_cast<json_value_t *>(json);
	allocate_ = false;
	return true;
}

Json::Json()
	: val_(new JsonValue(nullptr))
{}

Json::Json(const std::string& str) 
	: val_(new JsonValue(str))
{}

Json::Json(const char* str) 
	: val_(new JsonValue(str)) 
{}

Json::Json(std::nullptr_t null) 
	: val_(new JsonValue(null)) 
{}

Json::Json(double value)
	: val_(new JsonValue(value)) 
{}
	
Json::Json(int value)
	: val_(new JsonValue(value)) 
{}

Json::Json(bool value)
	: val_(new JsonValue(value)) 
{}

Json::Json(const Object& obj)
	: val_(new JsonValue(obj)) 
{}

Json::Json(JsonValue* val)
	: val_(val)
{}

Json::Json(Json&& other)
{
	val_ = other.val_;
	parent_ = other.parent_;
	other.val_ = nullptr;   
	other.parent_ = nullptr; 
}

Json& Json::operator=(Json&& other)
{
	if (this == &other)
	{
		return *this;
	}
	// destroey the resources the Json hold
	delete val_;
	
	val_ = other.val_;
	parent_ = other.parent_;
	other.val_ = nullptr;
	other.parent_ = nullptr; 

	return *this;
}

Json::~Json() 
{
	if(val_) 
	{
		delete val_;
	}
}

Json Json::parse(const std::string &str)
{
    return Json(str);
}

Json Json::parse(const std::ifstream& stream)
{
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return Json(buffer.str());
}

Json& Json::operator[](const std::string& key)
{
	Json* parent = this->parent_;
	if(parent)
	{
		const json_value_t* sub_obj = parent->val_->create_sub_object(parent->key_);
		val_->assign(sub_obj);
	} 
	if(val_->type() == JSON_VALUE_NULL)
	{
		val_->to_object();
	}
	auto it = object_.find(key);
	if(it != object_.end())
	{
		return it->second;
	}
	Json js(new JsonValue);  // empty
	assert(js.empty());
	this->key_ = key;
	js.parent_ = this;
	auto ret = object_.emplace(key, std::move(js));
	return ret.first->second;
}

Json& Json::operator=(int val)
{
	return operator=(static_cast<double>(val));
}

Json& Json::operator=(double val)
{
	Json* json = this->parent_;
	assert(json->type() == JSON_VALUE_OBJECT);
	json->push_back(json->key_, val);
	return *this;
}

void Json::push_back(const std::string& key, int val)
{
	val_->push_back(key, val);
}

void Json::push_back(const std::string& key, double val)
{
	val_->push_back(key, val);
}

const std::string Json::dump() const
{
    return dump(0);
}

const std::string Json::dump(int spaces) const
{
	std::string str;
    str.reserve(64);
    JsonValue::value_convert(val_->json(), spaces, 0, &str);
    return str;
}

int Json::type() const
{
	return val_->type();
}

bool Json::empty() const
{
	return val_->empty();
}

void JsonValue::value_convert(const json_value_t *val, int spaces, int depth, std::string* out_str)
{
	if(val == nullptr || out_str == nullptr) return;
	switch (json_value_type(val))
	{
	case JSON_VALUE_STRING:
		string_convert(json_value_string(val), out_str);
		break;
	case JSON_VALUE_NUMBER:
		number_convert(json_value_number(val), out_str);
		break;
	case JSON_VALUE_OBJECT:
		object_convert(json_value_object(val), spaces, depth, out_str);
		break;
	case JSON_VALUE_ARRAY:
		array_convert(json_value_array(val), spaces, depth, out_str);
		break;
	case JSON_VALUE_TRUE:
		out_str->append("true");
		break;
	case JSON_VALUE_FALSE:
		out_str->append("false");
		break;
	case JSON_VALUE_NULL:
        out_str->append("null");
		break;
	}
}

void JsonValue::string_convert(const char *str, std::string* out_str)
{
	out_str->append("\"");
	while (*str)
	{
		switch (*str)
		{
		case '\r':
			out_str->append("\\r");
			break;
		case '\n':
			out_str->append("\\n");
			break;
		case '\f':
			out_str->append("\\f");
			break;
		case '\b':
			out_str->append("\\b");
			break;
		case '\"':
			out_str->append("\\\"");
			break;
		case '\t':
			out_str->append("\\t");
			break;
		case '\\':
			out_str->append("\\\\");
			break;
		default:
			out_str->push_back(*str);
			break;
		}
		str++;
	}
	out_str->append("\"");
}

void JsonValue::number_convert(double number, std::string* out_str)
{
    std::ostringstream oss;
	long long integer = number;
	if (integer == number)
        oss << integer;
	else
        oss << number;

    out_str->append(oss.str());
}

void JsonValue::array_convert_not_format(const json_array_t *arr, std::string* out_str)
{
	const json_value_t *val;
	int n = 0;
    
	out_str->append("[");
	json_array_for_each(val, arr)
	{
		if (n != 0)
        {
            out_str->append(",");
        }
		n++;
		value_convert(val, 0, 0, out_str);
	}
	out_str->append("]");
}

void JsonValue::array_convert(const json_array_t *arr, int spaces, int depth, std::string* out_str)
{
	if(spaces == 0) 
	{
		return array_convert_not_format(arr, out_str);
	}
	const json_value_t *val;
	int n = 0;
	int i;
    std::string padding(spaces, ' ');
	out_str->append("[\n");
	json_array_for_each(val, arr)
	{
		if (n != 0)
        {
            out_str->append(",\n");
        }
		n++;
		for (i = 0; i < depth + 1; i++)
        {
            out_str->append(padding);
        }
		value_convert(val, spaces, depth + 1, out_str);
	}

	out_str->append("\n");
	for (i = 0; i < depth; i++)
    {
        out_str->append(padding);
    }
	out_str->append("]");
}



void JsonValue::object_convert_not_format(const json_object_t *obj, std::string* out_str)
{
	const char *name;
	const json_value_t *val;
	int n = 0;

	out_str->append("{");
	json_object_for_each(name, val, obj)
	{
		if (n != 0)
        {
            out_str->append(",");
        }
		n++;
        out_str->append("\"");
        out_str->append(name);
        out_str->append("\":");
		value_convert(val, 0, 0, out_str);
	}
	out_str->append("}");
}

void JsonValue::object_convert(const json_object_t *obj, int spaces, int depth, std::string* out_str)
{
	if(spaces == 0) 
	{
		return object_convert_not_format(obj, out_str);
	}
	const char *name;
	const json_value_t *val;
	int n = 0;
	int i;
    std::string padding(spaces, ' ');
	out_str->append("{\n");
	json_object_for_each(name, val, obj)
	{
		if (n != 0)
        {
            out_str->append(",\n");
        }
		n++;
		for (i = 0; i < depth + 1; i++)
        {   
            out_str->append(padding);
        }
        out_str->append("\"");
        out_str->append(name);
        out_str->append("\": ");
		value_convert(val, spaces, depth + 1, out_str);
	}

	out_str->append("\n");
	for (i = 0; i < depth; i++)
    {
        out_str->append(padding);
    }
	out_str->append("}");
}

}  // namespace wfrest