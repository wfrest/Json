#include "Json.h"
#include <fstream>
#include <iostream>
using namespace wfrest;

void create_by_operator()
{
    std::cout << "1. Creating empty structure..." << std::endl;
    // create an empty structure (null)
    Json data;
    std::cout << "2. Printing empty structure..." << std::endl;
    std::cout << "empty structure is " << data << std::endl;

    std::cout << "3. Adding pi value..." << std::endl;
    // add a number that is stored as double (note the implicit conversion of j
    // to an object)
    data["pi"] = 3.141;

    std::cout << "4. Adding happy value..." << std::endl;
    // add a Boolean that is stored as bool
    data["happy"] = true;

    std::cout << "5. Adding name value..." << std::endl;
    // add a string that is stored as std::string
    data["name"] = "chanchan";

    std::cout << "6. Adding nothing value..." << std::endl;
    // add another null object by passing nullptr
    data["nothing"] = nullptr;

    std::cout << "7. Adding nested answer.everything value..." << std::endl;
    // add an object inside the object - using a different approach
    Json answer;
    answer["everything"] = 42;
    data["answer"] = answer;
    
    std::cout << "8. Printing final structure..." << std::endl;
    std::cout << data << std::endl;
    
    std::cout << "9. Function completed successfully!" << std::endl;
}

void create_by_push_back_object()
{
    Json data;

    data.push_back("pi", 3.141);
    data.push_back("happy", true);
    data.push_back("name", "chanchan");
    data.push_back("nothing", nullptr);
    Json answer;
    answer.push_back("everything", 42);
    data.push_back("answer", answer);
    std::cout << data << std::endl;
}

void create_by_push_back_array()
{
    Json data;
    data.push_back(3.141);
    data.push_back(true);
    data.push_back("chanchan");
    data.push_back(nullptr);
    Json arr;
    arr.push_back(42);
    arr.push_back("answer");
    data.push_back(arr);
    std::cout << data << std::endl;
}

int main()
{
    create_by_operator();

    create_by_push_back_object();

    create_by_push_back_array();
}
