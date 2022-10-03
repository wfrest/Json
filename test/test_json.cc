#include "Json.h"
#include <fstream>
#include <iostream>

using namespace wfrest;

void test01()
{
    std::cout << "test01" << std::endl;
    std::ifstream f("example.json");
    Json data = Json::parse(f);
    std::cout << data.dump() << std::endl;
}

void test02()
{
    std::cout << "test02" << std::endl;
    Json data = Json::parse(R"(
    {
        "pi": 3.141,
        "happy": true
    }
    )");
    std::cout << data.dump() << std::endl;
}

void test03()
{
    std::cout << "test03" << std::endl;
    Json data;
    std::cout << data.dump() << std::endl;
}

void test04()
{
    std::cout << "test04" << std::endl;
    Json data;
    data["test"] = 1;
    // todo : fix
    std::cout << data << std::endl;
}

int main()
{
    // test01();
    // test02();
    // test03();
    test04();
}