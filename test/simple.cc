#include "Json.h"
#include "test/test_utils.h"
#include <algorithm>
#include <iostream>

TEST(SimpleTest, BasicJsonOperations) {
    Json obj;
    obj["key1"] = 1;
    obj["key2"] = 2.0;
    obj["key3"] = true;
    obj["key4"] = "string";
    obj["key5"] = nullptr;

    ASSERT_TRUE(obj.is_object());
    ASSERT_TRUE(obj.at("key1").is_number());
    ASSERT_TRUE(obj.at("key2").is_number());
    ASSERT_TRUE(obj.at("key3").is_boolean());
    ASSERT_TRUE(obj.at("key4").is_string());
    ASSERT_TRUE(obj.at("key5").is_null());

    ASSERT_EQ(obj.at("key1").get<int>(), 1);
    ASSERT_EQ(obj.at("key2").get<double>(), 2.0);
    ASSERT_EQ(obj.at("key3").get<bool>(), true);
    ASSERT_EQ(obj.at("key4").get<std::string>(), "string");
}

TEST(SimpleTest, JsonSerialization) {
    Json obj;
    obj["key1"] = 1;
    obj["key2"] = "string";
    obj["key3"] = true;
    obj["key4"] = nullptr;

    Json arr;
    arr.push_back(Json(1));
    arr.push_back(Json(2));
    obj["array"] = arr;

    Json nested;
    nested["a"] = "value";
    obj["object"] = nested;

    std::string json_str = obj.dump();
    Json parsed = Json::parse(json_str);

    ASSERT_TRUE(parsed.is_object());
    ASSERT_TRUE(parsed.at("array").is_array());
    ASSERT_TRUE(parsed.at("object").is_object());
}

// Add the main function for our test framework
MAIN_TEST() 