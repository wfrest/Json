#include "Json.h"

namespace wfrest
{

// inner class 
class JsonValue
{
public:
    JsonValue() 
        : json_(json_value_create(JSON_VALUE_NULL)) 
	{}

	explicit JsonValue(std::nullptr_t)
		: json_(json_value_create(JSON_VALUE_NULL)) 
	{}

    JsonValue(double value) 
		: json_(json_value_create(JSON_VALUE_NUMBER, value)) 
	{}

    JsonValue(int value)
		: json_(json_value_create(JSON_VALUE_NUMBER, static_cast<double>(value))) 
	{}

    JsonValue(bool value)
		: json_(value ? json_value_create(JSON_VALUE_TRUE) : json_value_create(JSON_VALUE_FALSE)) 
	{}

    // Json(const Array &values);
    // Json(const Object &values);

    explicit JsonValue(const std::string& str) 
        : json_(json_value_parse(str.c_str())) {}

    explicit JsonValue(const char* str) 
        : json_(json_value_parse(str)) {}

    ~JsonValue() 
    {
        if(json_) 
        {
            json_value_destroy(json_);
        }
    }

    void operator=(const std::string& str);

    const json_value_t* json() const { return json_; }
    void set_key(const std::string& key) { key_ = key; }
    void set_key(std::string&& key) { key_ = std::move(key); }
    
private:
    std::string key_;
    json_value_t *json_;
};

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

// Json::Json(const Array &values)
// 	: val_(new JsonValue(str)) 
// {}

// Json::Json(const Object &values)
// 	: val_(new JsonValue(str)) 
// {}

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
	val_->set_key(key);
	return *this;
}

Json& Json::operator[](std::string&& key)
{
	val_->set_key(std::move(key));
	return *this;
}

std::string Json::dump()
{
    return dump(0);
}

std::string Json::dump(int spaces)
{
	std::string str;
    str.reserve(64);
    value_convert(val_->json(), spaces, 0, &str);
    return str;
}

void Json::value_convert(const json_value_t *val, int spaces, int depth, std::string* out_str)
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

void Json::string_convert(const char *str, std::string* out_str)
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

void Json::number_convert(double number, std::string* out_str)
{
    std::ostringstream oss;
	long long integer = number;
	if (integer == number)
        oss << integer;
	else
        oss << number;

    out_str->append(oss.str());
}

void Json::array_convert_not_format(const json_array_t *arr, std::string* out_str)
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

void Json::array_convert(const json_array_t *arr, int spaces, int depth, std::string* out_str)
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



void Json::object_convert_not_format(const json_object_t *obj, std::string* out_str)
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

void Json::object_convert(const json_object_t *obj, int spaces, int depth, std::string* out_str)
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