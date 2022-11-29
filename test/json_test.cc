#include "Json.h"
#include <gtest/gtest.h>

using namespace wfrest;

TEST(JsonTest, null)
{
    Json data;
    EXPECT_EQ(data.type(), JSON_VALUE_NULL);
    EXPECT_EQ(data.dump(), "null");
}

TEST(JsonTest, arr_push)
{
    Json data;
    data.push_back(1);
    data.push_back(nullptr);
    data.push_back("string");
    data.push_back(true);
    data.push_back(false);
    EXPECT_EQ(data.dump(), R"([1,null,"string",true,false])");
}

TEST(JsonTest, arr_search)
{
    Json data;
    data.push_back(1);        // 0
    data.push_back(2.1);      // 1
    data.push_back(nullptr);  // 2
    data.push_back("string"); // 3
    data.push_back(true);     // 4
    data.push_back(false);    // 5
    Json::Object obj;
    obj["123"] = 12;
    obj["123"]["1"] = "test";
    // todo : we need a move interface
    // we copy here
    data.push_back(obj); // 6
    EXPECT_EQ(data[0].get<int>(), 1);
    EXPECT_EQ(data[1].get<double>(), 2.1);
    EXPECT_EQ(data[2].get<std::nullptr_t>(), nullptr);
    EXPECT_EQ(data[3].get<std::string>(), "string");
    EXPECT_EQ(data[4].get<bool>(), true);
    EXPECT_EQ(data[5].get<bool>(), false);
    // std::cout << data[6] << std::endl;
    // std::cout << data[6].get<Json::Object>().dump() << std::endl;
    EXPECT_EQ(data[6].get<Json::Object>().dump(), R"({"123":12})");

    // implicit conversion
    int a = data[0];
    EXPECT_EQ(a, 1);
    double b = data[1];
    EXPECT_EQ(b, 2.1);
    std::nullptr_t c = data[2];
    EXPECT_EQ(c, nullptr);
    std::string d = data[3];
    EXPECT_EQ(d, "string");
    bool e = data[4];
    EXPECT_EQ(e, true);
    bool f = data[5];
    EXPECT_EQ(f, false);
}

TEST(JsonTest, is_type)
{
    Json null1 = nullptr;
    EXPECT_EQ(null1.type(), JSON_VALUE_NULL);
    EXPECT_TRUE(null1.is_null());

    Json num1 = 1;
    EXPECT_EQ(num1.type(), JSON_VALUE_NUMBER);
    EXPECT_TRUE(num1.is_number());

    Json num2 = 1.0;
    EXPECT_EQ(num2.type(), JSON_VALUE_NUMBER);
    EXPECT_TRUE(num2.is_number());

    Json bool1 = true;
    EXPECT_EQ(bool1.type(), JSON_VALUE_TRUE);
    EXPECT_TRUE(bool1.is_boolean());

    Json bool2 = false;
    EXPECT_EQ(bool2.type(), JSON_VALUE_FALSE);
    EXPECT_TRUE(bool2.is_boolean());

    Json obj1 = Json::Object();
    EXPECT_EQ(obj1.type(), JSON_VALUE_OBJECT);
    EXPECT_TRUE(obj1.is_object());

    Json arr1 = Json::Array();
    EXPECT_EQ(arr1.type(), JSON_VALUE_ARRAY);
    EXPECT_TRUE(arr1.is_array());
}

TEST(JsonTest, const_operator)
{
    const Json js(
        R"({"test1":false,"test2":true,"test3":"string","test4":null})");
    EXPECT_EQ(js["test1"].get<bool>(), false);
    EXPECT_EQ(js["test3"].get<std::string>(), "string");
}

// TEST(JsonTest, create)
// {
//     Json data = {"123", 1};
// }

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
