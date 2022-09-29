#include <iostream>
#include "json.hpp"
using json = nlohmann::json;

void test01()
{
    json j;
    // null
    std::cout << j.dump() << std::endl;
}

int main()
{
    test01();
    return 0;
}