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
    data["test"] = 1.0;
    std::cout << data << std::endl;
}

void test05()
{
    std::cout << "test05" << std::endl;
    Json data;
    data["test"]["2"] = 3.0;
    std::cout << data << std::endl;
}

void test06()
{
    std::cout << "test06" << std::endl;
    Json data;
    data["test"] = 1.0;
    data["test1"] = 2.0;
    std::cout << data << std::endl;
}

void test07()
{
    std::cout << "test07" << std::endl;
    Json data;
    std::cout << data << std::endl;
    data["test"] = 1.0;
    data["test1"]["test2"] = 2.0;
    std::cout << data << std::endl;
}

void test08()
{
    Json data;
    data.push_back("test", 2.0);
    data.push_back("test1", 1111);
    std::cout << data << std::endl;
}


int main()
{
    // test01();
    // test02();
    // test03();
    // test04();
    // test05();
    // test06();
    // test07();
    test08();
}