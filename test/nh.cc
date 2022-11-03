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

void test03()
{
    auto j = json::parse(R"({"happy": true, "pi": 3.141})");
    std::cout << j["happy"].get<std::string>();
}

int main()
{
    // test01();
    test03();
    return 0;
}