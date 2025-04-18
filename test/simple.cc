#include "Json.h"
#include "test/test_utils.h"
#include <algorithm>
#include <iostream>

using namespace wfrest;

TEST(SimpleTest, BasicJsonOperations) {
    std::cout << "\n== TEST START: BasicJsonOperations ==\n" << std::endl;
    
    Json obj;
    std::cout << "Created empty JSON object" << std::endl;
    
    std::cout << "Setting key1 = 1" << std::endl;
    obj["key1"] = 1;
    
    std::cout << "Setting key2 = 2.0" << std::endl;
    obj["key2"] = 2.0;
    
    std::cout << "Setting key3 = true" << std::endl;
    obj["key3"] = true;
    
    std::cout << "Setting key4 = \"string\"" << std::endl;
    obj["key4"] = "string";
    
    std::cout << "Setting key5 = nullptr" << std::endl;
    obj["key5"] = nullptr;

    std::cout << "Getting key1 via at()" << std::endl;
    Json key1 = obj.at("key1");
    std::cout << "Type of key1: " << key1.type() << std::endl;
    std::cout << "Is key1 a number? " << (key1.is_number() ? "yes" : "no") << std::endl;
    std::cout << "Value of key1: " << key1.get<int>() << std::endl;

    std::cout << "Checking if obj is an object: " << (obj.is_object() ? "yes" : "no") << std::endl;
    
    std::cout << "Testing assertions..." << std::endl;
    
    ASSERT_TRUE(obj.is_object());
    ASSERT_TRUE(obj.at("key1").is_number());
    ASSERT_TRUE(obj.at("key2").is_number());
    ASSERT_TRUE(obj.at("key3").is_boolean());
    ASSERT_TRUE(obj.at("key4").is_string());
    ASSERT_TRUE(obj.at("key5").is_null());

    ASSERT_EQ(obj.at("key1").get<int>(), 1);
    ASSERT_EQ(obj.at("key2").get<double>(), 2.0);
    ASSERT_EQ(obj.at("key3").get<bool>(), true);
    ASSERT_EQ(obj.at("key4").get<std::string>(), "string");
    
    std::cout << "\n== TEST END: BasicJsonOperations ==\n" << std::endl;
}

TEST(SimpleTest, JsonSerialization) {
    std::cout << "\n== TEST START: JsonSerialization ==\n" << std::endl;
    
    Json obj;
    std::cout << "Created empty JSON object" << std::endl;
    
    std::cout << "Setting basic properties (key1, key2, key3, key4)" << std::endl;
    obj["key1"] = 1;
    obj["key2"] = "string";
    obj["key3"] = true;
    obj["key4"] = nullptr;

    std::cout << "Creating array and adding two elements" << std::endl;
    Json arr = Json::create_array();
    arr.push_back(Json(1));
    arr.push_back(Json(2));
    std::cout << "Array created. Is array? " << (arr.is_array() ? "yes" : "no") << std::endl;
    
    std::cout << "Setting arr to obj[\"array\"]" << std::endl;
    obj["array"] = arr;

    std::cout << "Creating nested object" << std::endl;
    Json nested = Json::create_object();
    nested["a"] = "value";
    std::cout << "Setting nested to obj[\"object\"]" << std::endl;
    obj["object"] = nested;

    std::cout << "Serializing to JSON string" << std::endl;
    std::string json_str = obj.dump();
    std::cout << "JSON string: " << json_str << std::endl;
    
    std::cout << "Parsing JSON string back to object" << std::endl;
    Json parsed = Json::parse(json_str);

    std::cout << "Checking types of parsed object" << std::endl;
    std::cout << "Is parsed an object? " << (parsed.is_object() ? "yes" : "no") << std::endl;
    
    Json array_elem = parsed.at("array");
    Json object_elem = parsed.at("object");
    
    std::cout << "Type of array_elem: " << array_elem.type() << std::endl;
    std::cout << "Is parsed[\"array\"] an array? " << (array_elem.is_array() ? "yes" : "no") << std::endl;
    
    std::cout << "Type of object_elem: " << object_elem.type() << std::endl;
    std::cout << "Is parsed[\"object\"] an object? " << (object_elem.is_object() ? "yes" : "no") << std::endl;

    ASSERT_TRUE(parsed.is_object());
    ASSERT_TRUE(parsed.at("array").is_array());
    ASSERT_TRUE(parsed.at("object").is_object());
    
    std::cout << "\n== TEST END: JsonSerialization ==\n" << std::endl;
}

int main() {
    std::cout << "=== Beginning JSON tests ===" << std::endl;
    
    bool all_passed = true;
    
    try {
        SimpleTest_BasicJsonOperations();
    } catch (const std::exception& e) {
        std::cerr << "BasicJsonOperations test failed: " << e.what() << std::endl;
        all_passed = false;
    }
    
    try {
        SimpleTest_JsonSerialization();
    } catch (const std::exception& e) {
        std::cerr << "JsonSerialization test failed: " << e.what() << std::endl;
        all_passed = false;
    }
    
    if (all_passed) {
        std::cout << "=== All tests passed! ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some tests failed! ===" << std::endl;
        return 1;
    }
}
