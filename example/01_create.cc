#include "Json.h"
#include <fstream>
using namespace wfrest;

void create_by_file()
{
    std::ofstream test_file("example.json");
    test_file << R"({"123" : 1})";
    test_file.close();

    std::ifstream f("example.json");
    Json data = Json::parse(f);
    std::cout << data << std::endl;
}

void create_by_string()
{
    Json data = Json::parse(R"(
    {
        "pi": 3.141,
        "happy": true
    }
    )");

    std::cout << data << std::endl;
}

void create_by_initailizer()
{
    // Json data = {{"pi", 3.141},        {"happy", true},
    //              {"pi", 3.141},        {"name", "chanchan"},
    //              {"nothing", nullptr}, {"123", Json::Object{"123", 123}}};
    // std::cout << data << std::endl;
}

int main()
{
    std::cout << "create json from file" << std::endl;
    create_by_file();

    std::cout << "create json from string" << std::endl;
    create_by_string();

    create_by_initailizer();
}
