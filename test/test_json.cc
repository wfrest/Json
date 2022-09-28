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

int main()
{
    test01();
    test02();
}