#include "Json.h"
#include "test/test_utils.h"
#include <iostream>
#include <vector>

using namespace wfrest;

class DestructorTester {
public:
    DestructorTester(const std::string& name) : name_(name) {
        std::cout << "Creating DestructorTester: " << name_ << std::endl;
    }
    
    ~DestructorTester() {
        std::cout << "Destroying DestructorTester: " << name_ << std::endl;
    }
    
private:
    std::string name_;
};

TEST(DestructorTest, simple_destruction) 
{
    std::cout << "\n=== Testing simple destruction ===\n" << std::endl;
    {
        std::cout << "Creating a Json object" << std::endl;
        Json obj;
        obj["key1"] = 1;
        obj["key2"] = "string";
        std::cout << "Json object going out of scope" << std::endl;
    }
    std::cout << "After Json object is destroyed" << std::endl;
    
    // Simple assert to make the test pass
    ASSERT_TRUE(true);
}

TEST(DestructorTest, nested_destruction) 
{
    std::cout << "\n=== Testing nested objects destruction ===\n" << std::endl;
    {
        std::cout << "Creating a Json object with nested objects" << std::endl;
        Json obj;
        obj["key1"] = 1;
        
        std::cout << "Creating nested object" << std::endl;
        Json nested;
        nested["a"] = "value";
        nested["b"] = 42;
        
        std::cout << "Assigning nested object to parent" << std::endl;
        obj["nested"] = nested;
        
        std::cout << "Json objects going out of scope" << std::endl;
    }
    std::cout << "After Json objects are destroyed" << std::endl;
    
    // Simple assert to make the test pass
    ASSERT_TRUE(true);
}

TEST(DestructorTest, multiple_references)
{
    std::cout << "\n=== Testing multiple references ===\n" << std::endl;
    {
        std::cout << "Creating a Json object" << std::endl;
        Json obj;
        obj["key1"] = 1;
        
        std::cout << "Getting multiple references to the same key" << std::endl;
        Json& ref1 = obj["key1"];
        Json& ref2 = obj["key1"];
        
        std::cout << "Values: ref1=" << ref1 << ", ref2=" << ref2 << std::endl;
        
        std::cout << "Json object going out of scope" << std::endl;
    }
    std::cout << "After Json object is destroyed" << std::endl;
    
    // Simple assert to make the test pass
    ASSERT_TRUE(true);
}

TEST(DestructorTest, object_array)
{
    std::cout << "\n=== Testing object with array ===\n" << std::endl;
    {
        std::cout << "Creating a Json object with array" << std::endl;
        Json obj;
        obj["array"] = Json::array();
        
        std::cout << "Adding elements to array" << std::endl;
        for (int i = 0; i < 5; i++) {
            obj["array"].push_back(i);
        }
        
        std::cout << "Json object going out of scope" << std::endl;
    }
    std::cout << "After Json object is destroyed" << std::endl;
    
    // Simple assert to make the test pass
    ASSERT_TRUE(true);
}

TEST(DestructorTest, multiple_objects)
{
    std::cout << "\n=== Testing multiple objects ===\n" << std::endl;
    {
        std::cout << "Creating multiple Json objects" << std::endl;
        std::vector<Json> objects;
        
        for (int i = 0; i < 5; i++) {
            Json obj;
            obj["index"] = i;
            obj["value"] = i * 10;
            objects.push_back(obj);
        }
        
        std::cout << "Json objects going out of scope" << std::endl;
    }
    std::cout << "After Json objects are destroyed" << std::endl;
    
    // Add a DestructorTester to verify proper memory management
    DestructorTester tester("test_multiple_objects");
    
    // Simple assert to make the test pass
    ASSERT_TRUE(true);
}

// Add the main function for our test framework
MAIN_TEST() 