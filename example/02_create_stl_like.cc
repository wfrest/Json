#include "Json.h"
#include <fstream>
using namespace wfrest;

void create_by_operator()
{
    // create an empty structure (null)
    Json data;
    std::cout << "empty structure is " << data << std::endl;

    // add a number that is stored as double (note the implicit conversion of j
    // to an object)
    data["pi"] = 3.141;

    // add a Boolean that is stored as bool
    data["happy"] = true;

    // add a string that is stored as std::string
    data["name"] = "chanchan";

    // add another null object by passing nullptr
    data["nothing"] = nullptr;

    // add an object inside the object
    data["answer"]["everything"] = 42;
    std::cout << data << std::endl;
}

void create_by_push_back_object()
{
    Json data;

    data.push_back("pi", 3.141);
    data.push_back("happy", true);
    data.push_back("name", "chanchan");
    data.push_back("nothing", nullptr);
    Json answer;
    answer.push_back("everything", 42);
    data.push_back("answer", answer);
    std::cout << data << std::endl;
}

int main()
{
    create_by_operator();

    create_by_push_back_object();
}
