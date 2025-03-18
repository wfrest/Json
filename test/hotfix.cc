#include "Json.h"
#include <iostream>
#include <cassert>

// Add a simple assertion macro
#define ASSERT(condition) \
    if (!(condition)) { \
        std::cerr << "Assertion failed: " #condition << std::endl; \
        return 1; \
    }

using namespace wfrest;

// Create a hotfix class to debug the number issue
class HotfixJson {
private:
    int value_;
    
public:
    HotfixJson(int val) : value_(val) {}
    
    int type() const {
        return JSON_VALUE_NUMBER;
    }
    
    bool is_number() const {
        return true;
    }
    
    int get_value() const {
        return value_;
    }
};

int main() {
    // Test regular JSON
    Json obj;
    obj["key1"] = 1;
    
    std::cout << "key1 type: " << obj["key1"].type() << std::endl;
    std::cout << "JSON_VALUE_NUMBER is: " << JSON_VALUE_NUMBER << std::endl;
    std::cout << "key1 is_number: " << (obj["key1"].is_number() ? "true" : "false") << std::endl;
    
    // Test hotfix class
    HotfixJson hotfix(1);
    std::cout << "hotfix type: " << hotfix.type() << std::endl;
    std::cout << "hotfix is_number: " << (hotfix.is_number() ? "true" : "false") << std::endl;
    
    // Add some assertions
    ASSERT(hotfix.is_number() == true);
    ASSERT(hotfix.type() == JSON_VALUE_NUMBER);
    
    // Test if JSON is_number works
    ASSERT(obj["key1"].is_number());
    ASSERT(hotfix.is_number());
    
    // Test assignment and serialization
    Json arr;
    arr.push_back(1);
    arr.push_back(2);
    obj["array"] = arr;
    
    std::string json_str = obj.dump();
    std::cout << "JSON string: " << json_str << std::endl;
    
    Json parsed = Json::parse(json_str);
    std::cout << "Parsed key1 is_number: " << (parsed["key1"].is_number() ? "true" : "false") << std::endl;
    std::cout << "Parsed array is_array: " << (parsed["array"].is_array() ? "true" : "false") << std::endl;
    
    ASSERT(parsed["key1"].is_number());
    ASSERT(parsed["array"].is_array());
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
} 