#include "JsonValue.h"

namespace wfrest
{

int JsonValue::type() const
{
	return json_value_type(json_);
}

int JsonValue::size() const
{
	if(type() == JSON_VALUE_ARRAY)
	{
		json_array_t* array = json_value_array(json_);
		return json_array_size(array);
	} else if(type() == JSON_VALUE_OBJECT)
	{
		json_object_t* obj = json_value_object(json_);
		return json_object_size(obj);
	}
	return 1;
}

bool JsonValue::empty() const
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

bool JsonValue::can_obj_push_back()
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

void JsonValue::push_back(const std::string& key, int val)
{
	this->push_back(key, static_cast<double>(val));
}

void JsonValue::push_back(const std::string& key, double val)
{
	if(!can_obj_push_back())
	{
		return;
	}
	json_object_t* obj = json_value_object(json_);
	json_object_append(obj, key.c_str(), JSON_VALUE_NUMBER, val);
}

void JsonValue::push_back(const std::string& key, bool val)
{
	if(!can_obj_push_back())
	{
		return;
	}
	json_object_t* obj = json_value_object(json_);
    if(val)
    {
        json_object_append(obj, key.c_str(), JSON_VALUE_TRUE);
    } else 
    {
        json_object_append(obj, key.c_str(), JSON_VALUE_FALSE);
    }
}

void JsonValue::push_back(const std::string& key, const std::string& val)
{
	if(!can_obj_push_back())
	{
		return;
	}
	json_object_t* obj = json_value_object(json_);
	json_object_append(obj, key.c_str(), JSON_VALUE_STRING, val.c_str());
}

void JsonValue::push_back(const std::string& key, const char* val)
{
	if(!can_obj_push_back())
	{
		return;
	}
	json_object_t* obj = json_value_object(json_);
	json_object_append(obj, key.c_str(), JSON_VALUE_STRING, val);
}

void JsonValue::push_back(const std::string& key, std::nullptr_t val)
{
	if(!can_obj_push_back())
	{
		return;
	}
	json_object_t* obj = json_value_object(json_);
	json_object_append(obj, key.c_str(), JSON_VALUE_NULL);
}

bool JsonValue::can_arr_push_back()
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

void JsonValue::push_back(int val)
{
	this->push_back(static_cast<double>(val));
}

void JsonValue::push_back(double val)
{
	if(!can_arr_push_back())
	{
		return;
	}
	json_array_t* arr = json_value_array(json_);
	json_array_append(arr, JSON_VALUE_NUMBER, val);
}

void JsonValue::push_back(bool val)
{
	if(!can_arr_push_back())
	{
		return;
	}
	json_array_t* arr = json_value_array(json_);
    if(val)
    {
		json_array_append(arr, JSON_VALUE_TRUE);
    } else 
    {
		json_array_append(arr, JSON_VALUE_FALSE);
    }
}

void JsonValue::push_back(const std::string& val)
{
	if(!can_arr_push_back())
	{
		return;
	}
	json_array_t* arr = json_value_array(json_);
	json_array_append(arr, JSON_VALUE_STRING, val.c_str());
}

void JsonValue::push_back(const char* val)
{
	if(!can_arr_push_back())
	{
		return;
	}
	json_array_t* arr = json_value_array(json_);
	json_array_append(arr, JSON_VALUE_STRING, val);
}

void JsonValue::push_back(std::nullptr_t val)
{
	if(!can_arr_push_back())
	{
		return;
	}
	json_array_t* arr = json_value_array(json_);
	json_array_append(arr, JSON_VALUE_NULL);
}

void JsonValue::to_object()
{
	// TODO : optimize
	json_value_destroy(json_);
	json_ = json_value_create(JSON_VALUE_OBJECT);
}

void JsonValue::to_array()
{
	json_value_destroy(json_);
	json_ = json_value_create(JSON_VALUE_ARRAY);
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