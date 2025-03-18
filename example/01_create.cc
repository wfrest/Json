#include "Json.h"
#include <fstream>
#include <utility>
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
    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();
    Json data = Json::parse(content);
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

void create_by_initailizer()
{
    // 在C++11中，我们无法直接使用花括号语法初始化，所以使用API方式构建
    // 但布局与原始花括号语法相似
    Json::Object obj;
    obj.push_back("null", nullptr);
    obj.push_back("integer", 1);
    obj.push_back("float", 1.3);
    obj.push_back("boolean", true);
    obj.push_back("string", "something");
    
    Json::Array arr;
    arr.push_back(1);
    arr.push_back(2);
    obj.push_back("array", arr);
    
    Json::Object nested_obj;
    nested_obj.push_back("key", "value");
    nested_obj.push_back("key2", "value2");
    obj.push_back("object", nested_obj);
    
    Json data = obj;
    std::cout << data << std::endl;
    
    // 注：在C++14或更高版本中，可以直接使用如下方式：
    /*
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
    */
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
}
