#include "Json.h"
#include <iostream>

using namespace wfrest;

void obj_iter()
{
    Json data;
    data["key1"] = 1;
    data["key2"] = 2.0;
    data["key3"] = true;
    for (Json::iterator it = data.begin(); it != data.end(); it++)
    {
        std::cout << it->key() << " : " << it->value() << std::endl;
    }
    for (auto it = data.begin(); it != data.end(); it++)
    {
        // equal to it->value()
        std::cout << *it << std::endl;
    }
    for (const auto& it : data)
    {
        std::cout << it.key() << " : " << it.value() << std::endl;
    }
}

void arr_iter()
{
    Json data;
    data.push_back(1);
    data.push_back(2.0);
    data.push_back(false);
    for (Json::iterator it = data.begin(); it != data.end(); it++)
    {
        std::cout << it->value() << std::endl;
    }
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        std::cout << (*it).value() << std::endl;
    }
    for (const auto& it : data)
    {
        std::cout << it << std::endl;
    }
}

int main()
{
    obj_iter();
    arr_iter();
}
