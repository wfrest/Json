#include "Json.h"
#include <iomanip>

using namespace wfrest;

void stringfy()
{
    Json data;
    data["key"]["chanchan"] = 1;
    // default compact json string
    // {"key":{"chanchan":1}}
    //
    // using member function
    // dump is same as str
    std::cout << data.dump() << std::endl;
    std::cout << data.str() << std::endl;
    // using free function
    std::cout << to_string(data) << std::endl;
    // streams
    std::ostringstream os;
    os << data;
    std::cout << os.str() << std::endl;
}

void stringfy_spaces()
{
    Json data;
    data["key"]["chanchan"] = 1;
    /*
     * formatted json
     * parmas is the number of spaces
    {
      "key": {
        "chanchan": 1
      }
    }
    */
    std::cout << data.dump(2) << std::endl;
    std::cout << data.str(2) << std::endl;
    std::cout << to_string(data, 2) << std::endl;
}

int main()
{
    std::cout << "default compact stringfy" << std::endl;
    stringfy();

    std::cout << "format stringy" << std::endl;
    stringfy_spaces();
}
