#include <gtest/gtest.h>
#include "Json.h"

using namespace wfrest;

TEST(JsonTest, null)
{
    Json data;
    EXPECT_EQ(data.type(), JSON_VALUE_NULL);
    EXPECT_EQ(data.dump(), "null");
}

TEST(JsonTest, one_level)
{
    Json data;
    data["test"] = 1.0;
    EXPECT_EQ(data.type(), JSON_VALUE_OBJECT);
    EXPECT_EQ(data.dump(), R"({"test":1})");
}

TEST(JsonTest, one_level_multi)
{
    Json data;
    data["test"] = 1.0;
    EXPECT_TRUE(data.has("test"));
    data["test1"] = 2;
    EXPECT_EQ(data.type(), JSON_VALUE_OBJECT);
    EXPECT_EQ(data.dump(), R"({"test":1,"test1":2})");
}

TEST(JsonTest, multi_level)
{
    Json data;
    data["test"]["test1"] = 1.0;
    EXPECT_EQ(data.type(), JSON_VALUE_OBJECT);
    EXPECT_EQ(data.dump(), R"({"test":{"test1":1}})");
}

TEST(JsonTest, multi_level_multi)
{
    Json data;
    data["test"]["test1"] = 1.0;
    data["test2"] = 2; 
    EXPECT_EQ(data.type(), JSON_VALUE_OBJECT);
    EXPECT_EQ(data.dump(), R"({"test":{"test1":1},"test2":2})");
}

TEST(JsonTest, push_other_type)
{
    Json data;
    data.push_back("test1", false);
    data.push_back("test2", true);
    data.push_back("test3", "string");
    data.push_back("test4", nullptr);
    EXPECT_EQ(data.dump(), R"({"test1":false,"test2":true,"test3":"string","test4":null})");
}

TEST(JsonTest, other_type)
{
    Json data;
    data["test1"] = false;
    data["test2"] = true;
    data["test3"] = "string";
    data["test4"] = nullptr;
    EXPECT_EQ(data.dump(), R"({"test1":false,"test2":true,"test3":"string","test4":null})");
}

TEST(JsonTest, clear)
{
    Json data;
    EXPECT_TRUE(data.empty());
    data["test1"] = false;
    EXPECT_EQ(data.size(), 1);
    EXPECT_FALSE(data.empty());
    data.clear();
    EXPECT_EQ(data.size(), 0);
    EXPECT_TRUE(data.empty());
    EXPECT_EQ(data.type(), JSON_VALUE_OBJECT);
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

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}