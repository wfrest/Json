#include "Json.h"
#include <gtest/gtest.h>

using namespace wfrest;

TEST(IterTest, object)
{
    Json data;
    data["key1"] = 1;
    data["key2"] = 2.0;
    data["key3"] = true;
    data["key4"] = "123";
    for (Json::iterator it = data.begin(); it != data.end(); it++)
    {
        std::cout << it.key() << std::endl;
    }
}

// int main(int argc, char** argv)
// {
//     testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
