#include "Json.h"
#include "JsonValue.h"
#include <cassert>

namespace wfrest
{

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
	Json js(new JsonValue);
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

Json& Json::operator=(bool val)
{
	Json* json = this->parent_;
	assert(json->type() == JSON_VALUE_OBJECT);
	json->push_back(json->key_, val);
	return *this;
}

Json& Json::operator=(const std::string& val)
{
	Json* json = this->parent_;
	assert(json->type() == JSON_VALUE_OBJECT);
	json->push_back(json->key_, val);
	return *this;
}

Json& Json::operator=(const char* val)
{
	Json* json = this->parent_;
	assert(json->type() == JSON_VALUE_OBJECT);
	json->push_back(json->key_, val);
	return *this;
}

Json& Json::operator=(std::nullptr_t val)
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

void Json::push_back(const std::string& key, bool val)
{
	val_->push_back(key, val);
}

void Json::push_back(const std::string& key, const std::string& val)
{
	val_->push_back(key, val);
}

void Json::push_back(const std::string& key, std::nullptr_t val)
{
	val_->push_back(key, val);
}

void Json::push_back(const std::string& key, const char* val)
{
	val_->push_back(key, val);
}

void Json::push_back(int val)
{
	val_->push_back(val);
}

void Json::push_back(double val)
{
	val_->push_back(val);
}

void Json::push_back(bool val)
{
	val_->push_back(val);
}

void Json::push_back(const std::string& val)
{
	val_->push_back(val);
}

void Json::push_back(const char* val)
{
	val_->push_back(val);
}

void Json::push_back(std::nullptr_t val)
{
	val_->push_back(val);
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

bool Json::is_null() const
{
	return type() == JSON_VALUE_NULL;
}

bool Json::is_number() const
{
	return type() == JSON_VALUE_NUMBER;
}

bool Json::is_boolean() const
{
	int type = this->type();
	return type == JSON_VALUE_TRUE || type == JSON_VALUE_FALSE;
}

bool Json::is_object() const
{
	return type() == JSON_VALUE_OBJECT;
}

bool Json::is_array() const
{
	return type() == JSON_VALUE_ARRAY;
}

bool Json::is_string() const
{
	return type() == JSON_VALUE_STRING;
}

int Json::size() const
{
	if(is_array())
	{
		json_array_t* array = json_value_array(val_->json());
		return json_array_size(array);
	} else if(is_object())
	{
		json_object_t* obj = json_value_object(val_->json());
		return json_object_size(obj);
	}
	return 1;
}

bool Json::empty() const
{
	return val_->empty();
}

void Json::clear()
{
	val_->to_object();
}

}  // namespace wfrest