#include "Json.h"
#include <iostream>

using namespace wfrest;

int main() {
    // Test 1: Basic array creation and manipulation
    std::cout << "=== Test 1: Basic Array Test ===" << std::endl;
    
    // Create an array with three elements
    Json arr = Json::create_array();
    arr.push_back(Json(1));
    arr.push_back(Json(2));
    arr.push_back(Json(3));
    
    std::cout << "Initial array: " << arr.dump() << std::endl;
    
    // Update the first element
    arr[0] = Json(99);
    
    std::cout << "After setting index 0: " << arr.dump() << std::endl;
    
    // Test 2: Object in array
    std::cout << "\n=== Test 2: Object in Array Test ===" << std::endl;
    
    // Create array with object
    Json arr2 = Json::create_array();
    arr2.push_back(Json(1));
    
    // Create and add object
    Json obj = Json::create_object();
    obj["name"] = Json("test");
    obj["value"] = Json(42);
    
    std::cout << "Object: " << obj.dump() << std::endl;
    
    // Add object to array
    arr2.push_back(obj);
    
    std::cout << "Array with object: " << arr2.dump() << std::endl;
    
    // Access elements
    Json first = arr2.at(0);
    Json second = arr2.at(1);
    
    std::cout << "First element: " << first.dump() << std::endl;
    std::cout << "Second element: " << second.dump() << std::endl;
    std::cout << "Second element type: " << second.type() << std::endl;
    std::cout << "Is second element an object? " << (second.is_object() ? "yes" : "no") << std::endl;
    
    return 0;
}
