#include "Json.h"
#include <fstream>
using namespace wfrest;

void create_json_file()
{
    std::ofstream test_file("example.json");
    test_file << R"({"123" : 1})";
    test_file.close();
}

void create_by_file_stream()
{
    std::ifstream f("example.json");
    Json data = Json::parse(f);
    std::cout << data << std::endl;
}

void create_by_file()
{
    FILE* fp = fopen("example.json", "r");
    Json data = Json::parse(fp);
    fclose(fp);
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

void create_by_empty_string()
{
    Json data = Json::parse("");
    if (data.is_valid())
    {
        std::cout << "valid" << std::endl;
    }
    else
    {
        std::cout << "invalid" << std::endl;
    }
    std::cout << data << std::endl;
}

void create_by_initailizer()
{
    Json data = Json::Object{
        {"null", nullptr},
        {"integer", 1},
        {"float", 1.3},
        {"boolean", true},
        {"string", "something"},
        {"array", Json::Array{1, 2}},
        {"object",
         Json::Object{
             {"key", "value"},
             {"key2", "value2"},
         }},
    };
    std::cout << data << std::endl;
}

int main()
{
    std::cout << "create example.json file" << std::endl;
    create_json_file();

    std::cout << "create json from file stream" << std::endl;
    create_by_file_stream();

    std::cout << "create json from string" << std::endl;
    create_by_string();

    std::cout << "create json from file" << std::endl;
    create_by_file();

    std::cout << "create by initializer list" << std::endl;
    create_by_initailizer();

    std::cout << "create by empty string" << std::endl;
    create_by_empty_string();
}
