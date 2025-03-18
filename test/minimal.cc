#include "Json.h"
#include "json_parser.h"
#include <iostream>

using namespace wfrest;

int main() {
    std::cout << "Creating empty JSON object..." << std::endl;
    Json obj;
    
    std::cout << "Type of empty object: " << obj.type() << std::endl;
    std::cout << "Is object? " << (obj.is_object() ? "true" : "false") << std::endl;
    
    std::cout << "Testing direct assignment..." << std::endl;
    Json direct;
    direct = 1;
    std::cout << "Type of direct: " << direct.type() << std::endl;
    std::cout << "direct is_number(): " << (direct.is_number() ? "true" : "false") << std::endl;
    
    std::cout << "Adding key1 with value 1..." << std::endl;
    obj["key1"] = 1;
    
    std::cout << "Type of obj after adding key1: " << obj.type() << std::endl;
    std::cout << "Is object now? " << (obj.is_object() ? "true" : "false") << std::endl;
    
    std::cout << "About to test key1..." << std::endl;
    
    Json& key1_ref = obj["key1"];
    std::cout << "Address of key1_ref: " << &key1_ref << std::endl;
    std::cout << "Type of key1_ref: " << key1_ref.type() << std::endl;
    std::cout << "JSON_VALUE_NUMBER is: " << JSON_VALUE_NUMBER << std::endl;
    
    bool is_num = key1_ref.is_number();
    std::cout << "key1 is_number(): " << (is_num ? "true" : "false") << std::endl;
    
    // Try direct assignment to the reference
    std::cout << "Directly assigning to key1_ref..." << std::endl;
    key1_ref = 42;
    std::cout << "Type of key1_ref after direct assignment: " << key1_ref.type() << std::endl;
    std::cout << "key1_ref is_number(): " << (key1_ref.is_number() ? "true" : "false") << std::endl;
    
    // Access it again to check
    std::cout << "Accessing key1 again..." << std::endl;
    Json& key1_ref2 = obj["key1"];
    std::cout << "Address of key1_ref2: " << &key1_ref2 << std::endl;
    std::cout << "Type of key1_ref2: " << key1_ref2.type() << std::endl;
    std::cout << "key1_ref2 is_number(): " << (key1_ref2.is_number() ? "true" : "false") << std::endl;
    
    // Try push_back instead
    std::cout << "Using push_back with key2..." << std::endl;
    Json tmp(1); // Create number value first
    obj.push_back("key2", tmp);
    Json& key2_ref = obj["key2"];
    std::cout << "Type of key2_ref: " << key2_ref.type() << std::endl;
    std::cout << "key2_ref is_number(): " << (key2_ref.is_number() ? "true" : "false") << std::endl;
    
    // 尝试打印类型名称
    std::cout << "Type names:" << std::endl;
    std::cout << "NULL: " << JSON_VALUE_NULL << std::endl;
    std::cout << "FALSE: " << JSON_VALUE_FALSE << std::endl;
    std::cout << "TRUE: " << JSON_VALUE_TRUE << std::endl;
    std::cout << "NUMBER: " << JSON_VALUE_NUMBER << std::endl;
    std::cout << "STRING: " << JSON_VALUE_STRING << std::endl;
    std::cout << "ARRAY: " << JSON_VALUE_ARRAY << std::endl;
    std::cout << "OBJECT: " << JSON_VALUE_OBJECT << std::endl;
    
    return 0;
} 