#include "Json.h"

namespace wfrest
{

Json::Json(Json&& other)
{
	parent_ = other.parent_;
	other.parent_ = nullptr; 
}

Json& Json::operator=(Json&& other)
{
	if (this == &other)
	{
		return *this;
	}
	parent_ = other.parent_;
	other.parent_ = nullptr; 
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

Json Json::create_incomplete_json()
{
	JsonValue val;  // json_ == nullptr
	Json js(std::move(val));
	return js;
}

Json& Json::operator[](const std::string& key)
{
	Json* parent = this->parent_;
	if(parent)
	{
		const json_value_t* sub_obj = parent->val_.create_sub_object(parent->key_);
		val_.assign(sub_obj);
	} 
	if(val_.type() == JSON_VALUE_NULL)
	{
		val_.to_object();
	}
	auto it = object_.find(key);
	if(it != object_.end())
	{
		return it->second;
	}
	Json imcomplet_js = create_incomplete_json();
	this->key_ = key;
	imcomplet_js.parent_ = this;
	auto ret = object_.emplace(key, std::move(imcomplet_js));
	return ret.first->second;
}

const std::string Json::dump() const
{
    return dump(0);
}

const std::string Json::dump(int spaces) const
{
	std::string str;
    str.reserve(64);
    JsonValue::value_convert(val_.json(), spaces, 0, &str);
    return str;
}

int Json::size() const
{
	if(is_array())
	{
		json_array_t* array = json_value_array(val_.json());
		return json_array_size(array);
	} else if(is_object())
	{
		json_object_t* obj = json_value_object(val_.json());
		return json_object_size(obj);
	}
	return 1;
}

}  // namespace wfrest