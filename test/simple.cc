#include "Json.h"
#include "test/test_utils.h"
#include <algorithm>
#include <iostream>

using namespace wfrest;

TEST(SimpleTest, BasicJsonOperations)
{
    std::cout << "Starting BasicJsonOperations test..." << std::endl;
    
    // Create a JSON object
    Json obj;
    std::cout << "Created empty JSON object" << std::endl;

    // 在创建临时变量，然后赋值
    Json tmp1(1);
    obj["key1"] = tmp1;
    std::cout << "Added key1 = 1" << std::endl;
    
    Json tmp2(2.0);
    obj["key2"] = tmp2;
    std::cout << "Added key2 = 2.0" << std::endl;
    
    Json tmp3(true);
    obj["key3"] = tmp3;
    std::cout << "Added key3 = true" << std::endl;
    
    Json tmp4("string");
    obj["key4"] = tmp4;
    std::cout << "Added key4 = string" << std::endl;
    
    Json tmp5(nullptr);
    obj["key5"] = tmp5;
    std::cout << "Added key5 = nullptr" << std::endl;

    // Test type checking
    std::cout << "Testing is_object()" << std::endl;
    ASSERT_TRUE(obj.is_object());
    
    std::cout << "Testing key1 is_number()" << std::endl;
    ASSERT_TRUE(obj["key1"].is_number());
    
    std::cout << "Testing key2 is_number()" << std::endl;
    ASSERT_TRUE(obj["key2"].is_number());
    
    std::cout << "Testing key3 is_boolean()" << std::endl;
    ASSERT_TRUE(obj["key3"].is_boolean());
    
    std::cout << "Testing key4 is_string()" << std::endl;
    ASSERT_TRUE(obj["key4"].is_string());
    
    std::cout << "Testing key5 is_null()" << std::endl;
    ASSERT_TRUE(obj["key5"].is_null());

    // Test value extraction using get<T>
    std::cout << "Testing key1 get<int>()" << std::endl;
    ASSERT_EQ(obj["key1"].get<int>(), 1);
    
    std::cout << "Testing key2 get<double>()" << std::endl;
    ASSERT_EQ(obj["key2"].get<double>(), 2.0);
    
    std::cout << "Testing key3 get<bool>()" << std::endl;
    ASSERT_EQ(obj["key3"].get<bool>(), true);
    
    std::cout << "Testing key4 get<std::string>()" << std::endl;
    ASSERT_EQ(obj["key4"].get<std::string>(), "string");

    // Test array operations
    std::cout << "Creating array" << std::endl;
    Json arr;
    
    std::cout << "Pushing 1 to array" << std::endl;
    Json item1(1);
    arr.push_back(item1);
    
    std::cout << "Pushing 2.0 to array" << std::endl;
    Json item2(2.0);
    arr.push_back(item2);
    
    std::cout << "Pushing true to array" << std::endl;
    Json item3(true);
    arr.push_back(item3);
    
    std::cout << "Testing is_array()" << std::endl;
    ASSERT_TRUE(arr.is_array());
    
    std::cout << "Testing array size" << std::endl;
    ASSERT_EQ(arr.size(), 3);
    
    // Use explicit size_t for array indices
    size_t idx0 = 0;
    size_t idx1 = 1;
    size_t idx2 = 2;
    
    std::cout << "Testing array[0] get<int>()" << std::endl;
    ASSERT_EQ(arr[idx0].get<int>(), 1);
    
    std::cout << "Testing array[1] get<double>()" << std::endl;
    ASSERT_EQ(arr[idx1].get<double>(), 2.0);
    
    std::cout << "Testing array[2] get<bool>()" << std::endl;
    ASSERT_EQ(arr[idx2].get<bool>(), true);
    
    std::cout << "BasicJsonOperations test completed" << std::endl;
}

TEST(SimpleTest, JsonSerialization)
{
    std::cout << "Starting JsonSerialization test..." << std::endl;
    
    Json obj;
    Json tmp1(1);
    obj["key1"] = tmp1;
    
    Json tmp2("string");
    obj["key2"] = tmp2;
    
    Json tmp3(true);
    obj["key3"] = tmp3;
    
    Json tmp4(nullptr);
    obj["key4"] = tmp4;
    
    // Create a nested array
    std::cout << "Creating nested array" << std::endl;
    Json arr;
    Json arrItem1(1);
    arr.push_back(arrItem1);
    Json arrItem2(2);
    arr.push_back(arrItem2);
    obj["array"] = arr;
    
    // Create a nested object
    std::cout << "Creating nested object" << std::endl;
    Json nested;
    Json nestedItem("value");
    nested["a"] = nestedItem;
    obj["object"] = nested;
    
    // Serialize to JSON string
    std::cout << "Dumping to JSON string" << std::endl;
    std::string json_str = obj.dump();
    std::cout << "JSON string: " << json_str << std::endl;
    
    // Parse it back
    std::cout << "Parsing JSON string" << std::endl;
    Json parsed = Json::parse(json_str);
    
    // Test if it's properly parsed
    std::cout << "Testing parsed object" << std::endl;
    ASSERT_TRUE(parsed.is_object());
    ASSERT_EQ(parsed.size(), 6);
    ASSERT_TRUE(parsed.has("key1"));
    ASSERT_TRUE(parsed.has("key2"));
    ASSERT_TRUE(parsed.has("key3"));
    ASSERT_TRUE(parsed.has("key4"));
    ASSERT_TRUE(parsed.has("array"));
    ASSERT_TRUE(parsed.has("object"));
    
    std::cout << "Testing nested array" << std::endl;
    ASSERT_TRUE(parsed["array"].is_array());
    ASSERT_EQ(parsed["array"].size(), 2);
    
    std::cout << "Testing nested object" << std::endl;
    ASSERT_TRUE(parsed["object"].is_object());
    ASSERT_TRUE(parsed["object"].has("a"));
    ASSERT_EQ(parsed["object"]["a"].get<std::string>(), "value");
    
    std::cout << "JsonSerialization test completed" << std::endl;
}

// Add the main function for our test framework
MAIN_TEST() 