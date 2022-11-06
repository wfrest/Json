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

void test04()
{
    auto j = json::parse(R"(["123", true, [123, false, "test"]])");
    // std::cout << j.dump() << std::endl;  // ["123",true,[123,false,"test"]]
    std::cout << j[0] << std::endl;  // "123"
    std::cout << j[2] << std::endl;  // [123,false,"test"]
    std::cout << j[2][0] << std::endl;  // 123
    j[2][0] = "111";
    std::cout << j[2][0] << std::endl;  // "111"
    std::cout << j[2][4] << std::endl;  // null
    std::cout << j[2].size() << std::endl;
}

int main()
{
    // test01();
    // test03();
    test04();
    return 0;
}