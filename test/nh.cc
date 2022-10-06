#include <iostream>
#include "json.hpp"
using json = nlohmann::json;

void test01()
{
    json j;
    if(j.empty())
    {
        std::cout << "empty" << std::endl;
    }
    // null
    std::cout << j.dump() << std::endl;
    j["data"] = 1;
    std::cout << j << std::endl;
    j.clear();
    std::cout << j << std::endl;
}

void test02()
{
    json empty_array_explicit = json::array();
}

int main()
{
    test01();
    return 0;
}