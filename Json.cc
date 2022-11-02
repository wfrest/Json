#include "Json.h"

namespace wfrest
{

Json::Json() 
	: root_(json_value_create(JSON_VALUE_NULL)),
	allocate_(true)
{}

Json::Json(const std::string& str)
	: root_(json_value_parse(str.c_str())),
	allocate_(true)
{}

Json::Json(const char* str)
	: root_(json_value_parse(str)),
	allocate_(true) 
{}

Json::Json(std::nullptr_t null)
	: root_(json_value_create(JSON_VALUE_NULL)),
	allocate_(true)
{}

Json::Json(double val)
	: root_(json_value_create(JSON_VALUE_NUMBER, val)),
	allocate_(true)
{}

Json::Json(int val)
	: root_(json_value_create(JSON_VALUE_NUMBER, static_cast<double>(val))),
	allocate_(true)
{}

Json::Json(bool val)
	: root_(val ? json_value_create(JSON_VALUE_TRUE) : json_value_create(JSON_VALUE_FALSE)),
	allocate_(true)
{}

Json::Json(const Array& val)
	: root_(json_value_create(JSON_VALUE_ARRAY)),
	allocate_(true)
{}

Json::Json(const Object& val)
	: root_(json_value_create(JSON_VALUE_OBJECT)),
	allocate_(true)
{}

Json::~Json()
{
	if (allocate_)
	{
		json_value_destroy(root_);
	}
}

Json::Json(const json_value_t *val, std::string&& key)
	: root_(const_cast<json_value_t *>(val)),
	allocate_(false),
	key_(std::move(key))
{}

Json::Json(const json_value_t *val, const std::string& key)
	: root_(const_cast<json_value_t *>(val)),
	allocate_(false),
	key_(key)
{}

Json::Json(const json_value_t *val)
	: root_(const_cast<json_value_t *>(val)),
	allocate_(false)
{}

Json::Json(const Empty&)
	: root_(nullptr),
	allocate_(false)
{}

Json::Json(Json&& other)
{
	root_ = other.root_;
	other.root_ = nullptr; 
}

Json& Json::operator=(Json&& other)
{
	if (this == &other)
	{
		return *this;
	}
	root_ = other.root_;
	other.root_ = nullptr; 
	return *this;
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

const std::string Json::dump() const
{
    return dump(0);
}

const std::string Json::dump(int spaces) const
{
	std::string str;
    str.reserve(64);
    value_convert(root_, spaces, 0, &str);
    return str;
}

Json Json::operator[](const std::string& key)
{
	if (!is_valid())
	{
		return Json(Empty());
	}
	const json_value_t *val = root_;
	this->to_object();
	// if exists
	json_object_t *obj = json_value_object(val);
	const json_value_t *res = json_object_find(key.c_str(), obj);
	if (res != nullptr)
	{
		return Json(res);
	}
	if (!is_object())
	{
		return Json(Empty());
	}
	if (!allocate_)
	{
		json_object_t *obj = json_value_object(val);
		val = json_object_append(obj, key_.c_str(), JSON_VALUE_OBJECT);
	}
	return Json(val, key);
}

bool Json::can_obj_push_back()
{
	if(this->type() == JSON_VALUE_NULL)
	{
		this->to_object();
	}
	else if(this->type() != JSON_VALUE_OBJECT)
	{
		return false;
	}
	return true;
}

void Json::push_back(const std::string& key, int val)
{
	this->push_back(key, static_cast<double>(val));
}

void Json::push_back(const std::string& key, double val)
{
	if(!can_obj_push_back())
	{
		return;
	}
	json_object_t* obj = json_value_object(root_);
	json_object_append(obj, key.c_str(), JSON_VALUE_NUMBER, val);
}

void Json::push_back(const std::string& key, bool val)
{
	if(!can_obj_push_back())
	{
		return;
	}
	json_object_t* obj = json_value_object(root_);
    if(val)
    {
        json_object_append(obj, key.c_str(), JSON_VALUE_TRUE);
    } else 
    {
        json_object_append(obj, key.c_str(), JSON_VALUE_FALSE);
    }
}

void Json::push_back(const std::string& key, const std::string& val)
{
	if(!can_obj_push_back())
	{
		return;
	}
	json_object_t* obj = json_value_object(root_);
	json_object_append(obj, key.c_str(), JSON_VALUE_STRING, val.c_str());
}

void Json::push_back(const std::string& key, const char* val)
{
	if(!can_obj_push_back())
	{
		return;
	}
	json_object_t* obj = json_value_object(root_);
	json_object_append(obj, key.c_str(), JSON_VALUE_STRING, val);
}

void Json::push_back(const std::string& key, std::nullptr_t val)
{
	if(!can_obj_push_back())
	{
		return;
	}
	json_object_t* obj = json_value_object(root_);
	json_object_append(obj, key.c_str(), JSON_VALUE_NULL);
}

bool Json::can_arr_push_back()
{
	if(this->type() == JSON_VALUE_NULL)
	{
		this->to_array();
	}
	else if(this->type() != JSON_VALUE_ARRAY)
	{
		return false;
	}
	return true;
}

void Json::push_back(int val)
{
	this->push_back(static_cast<double>(val));
}

void Json::push_back(double val)
{
	if(!can_arr_push_back())
	{
		return;
	}
	json_array_t* arr = json_value_array(root_);
	json_array_append(arr, JSON_VALUE_NUMBER, val);
}

void Json::push_back(bool val)
{
	if(!can_arr_push_back())
	{
		return;
	}
	json_array_t* arr = json_value_array(root_);
    if(val)
    {
		json_array_append(arr, JSON_VALUE_TRUE);
    } else 
    {
		json_array_append(arr, JSON_VALUE_FALSE);
    }
}

void Json::push_back(const std::string& val)
{
	if(!can_arr_push_back())
	{
		return;
	}
	json_array_t* arr = json_value_array(root_);
	json_array_append(arr, JSON_VALUE_STRING, val.c_str());
}

void Json::push_back(const char* val)
{
	if(!can_arr_push_back())
	{
		return;
	}
	json_array_t* arr = json_value_array(root_);
	json_array_append(arr, JSON_VALUE_STRING, val);
}

void Json::push_back(std::nullptr_t val)
{
	if(!can_arr_push_back())
	{
		return;
	}
	json_array_t* arr = json_value_array(root_);
	json_array_append(arr, JSON_VALUE_NULL);
}

int Json::size() const
{
	if(type() == JSON_VALUE_ARRAY)
	{
		json_array_t* array = json_value_array(root_);
		return json_array_size(array);
	} else if(type() == JSON_VALUE_OBJECT)
	{
		json_object_t* obj = json_value_object(root_);
		return json_object_size(obj);
	}
	return 1;
}

bool Json::empty() const
{
	switch (type())
	{
		case JSON_VALUE_NULL:
		{
			// null values are empty
			return true;
		}
		case JSON_VALUE_ARRAY:
		case JSON_VALUE_OBJECT:
		{
			return size() == 0;			
		}
	default:
		// all other types are nonempty
		return false;
	}
}

void Json::clear() 
{
	if (allocate_)
	{
		json_value_destroy(root_);
		root_ = json_value_create(JSON_VALUE_OBJECT);
	}
}

void Json::to_object()
{
	if (allocate_ && is_null())
	{
		json_value_destroy(root_);
		root_ = json_value_create(JSON_VALUE_OBJECT);
	}
}

void Json::to_array()
{
	if (allocate_ && is_null())
	{
		json_value_destroy(root_);
		root_ = json_value_create(JSON_VALUE_ARRAY);
	}
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