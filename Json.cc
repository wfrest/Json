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
	: root_(json_value_create(JSON_VALUE_OBJECT)),
	allocate_(true)
{}

Json::Json(const Object& val)
	: root_(json_value_create(JSON_VALUE_ARRAY)),
	allocate_(true)
{}

Json::~Json()
{
	json_value_destroy(root_);
}

Json::Json(bool allocate) 
	: root_(nullptr), 
	allocate_(allocate)
{}

Json::Json(json_object_t* obj)
	: allocate_(false)
{
	
}

Json Json::operator[](const std::string& key)
{
	if (is_object()) 
	{
		return Json(false);
	}
	json_object_t *obj = json_value_object(root_);
	json_object_append(obj, key.c_str(), JSON_VALUE_OBJECT);
	return Json(obj);
}

}  // namespace wfrest