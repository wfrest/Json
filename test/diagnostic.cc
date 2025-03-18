#include "Json.h"
#include "json_parser.h"
#include "test/test_utils.h"
#include <iostream>

using namespace wfrest;

TEST(DiagnosticTest, TypePreservation)
{
    std::cout << "Testing key1 assignment and type checking:" << std::endl;
    
    // Create a JSON object
    Json obj;
    std::cout << "Initial obj type: " << obj.type() << std::endl;
    
    // Test direct assignment
    obj["key1"] = 1;
    std::cout << "After assignment, obj[\"key1\"] type: " << obj["key1"].type() << std::endl;
    std::cout << "key1 is_number: " << (obj["key1"].is_number() ? "true" : "false") << std::endl;
    
    // Test array
    Json arr;
    arr.push_back(1);
    // Use explicit size_t for index to avoid ambiguity
    size_t index = 0;
    std::cout << "Array element type: " << arr[index].type() << std::endl;
    std::cout << "arr[0] is_number: " << (arr[index].is_number() ? "true" : "false") << std::endl;
    std::cout << "arr is_array: " << (arr.is_array() ? "true" : "false") << std::endl;
    
    // Test serialization
    std::string json_str = obj.dump();
    std::cout << "Serialized object: " << json_str << std::endl;
    
    Json parsed = Json::parse(json_str);
    std::cout << "Parsed key1 type: " << parsed["key1"].type() << std::endl;
    std::cout << "Parsed key1 is_number: " << (parsed["key1"].is_number() ? "true" : "false") << std::endl;
}

// Add the main function for our test framework
MAIN_TEST()

int main() {
    // Test 1: Check if direct assignment properly transfers type information
    std::cout << "Test 1: Direct Assignment Test" << std::endl;
    Json obj;
    std::cout << "Created empty JSON object" << std::endl;
    std::cout << "Type: " << obj.type() << std::endl;
    std::cout << "Is object: " << (obj.is_object() ? "true" : "false") << std::endl;
    
    // Add a number directly
    obj["key1"] = 1;
    std::cout << "Added key1 = 1 directly using operator=" << std::endl;
    std::cout << "Type of obj: " << obj.type() << std::endl;
    std::cout << "key1 type: " << obj["key1"].type() << std::endl;
    std::cout << "key1 is_number: " << (obj["key1"].is_number() ? "true" : "false") << std::endl;
    
    // Test 2: Array serialization test
    std::cout << "\nTest 2: Array Test" << std::endl;
    Json arr;
    arr.push_back(Json(123));
    
    size_t index = 0;
    std::cout << "arr[0] type: " << arr[index].type() << std::endl;
    std::cout << "arr[0] is_number: " << (arr[index].is_number() ? "true" : "false") << std::endl;
    
    // Test 3: Serialization test
    std::cout << "\nTest 3: Serialization Test" << std::endl;
    std::string json_str = obj.dump();
    std::cout << "JSON string: " << json_str << std::endl;
    
    Json parsed = Json::parse(json_str);
    std::cout << "Parsed key1 type: " << parsed["key1"].type() << std::endl;
    std::cout << "Parsed key1 is_number: " << (parsed["key1"].is_number() ? "true" : "false") << std::endl;
    
    // Add array to object
    Json nestedArr;
    nestedArr.push_back(1);
    obj["array"] = nestedArr;
    
    json_str = obj.dump();
    std::cout << "JSON with array: " << json_str << std::endl;
    
    Json parsedWithArray = Json::parse(json_str);
    std::cout << "parsedWithArray[array] type: " << parsedWithArray["array"].type() << std::endl;
    std::cout << "parsedWithArray[array] is_array: " << (parsedWithArray["array"].is_array() ? "true" : "false") << std::endl;
    
    return 0;
} 