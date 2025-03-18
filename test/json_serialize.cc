#include "Json.h"
#include "test/test_utils.h"
#include <sstream>

using namespace wfrest;

// 测试基本序列化
TEST(JsonSerializeTest, BasicSerialization)
{
    // 序列化简单对象
    JsonObject obj;
    obj.set("string", "value")
       .set("number", 42)
       .set("boolean", true)
       .set("null", nullptr);
    
    std::string serialized = obj.dump();
    ASSERT_EQ(serialized, R"({"string":"value","number":42,"boolean":true,"null":null})");
    
    // 序列化简单数组
    JsonArray arr;
    arr.push_back(1)
       .push_back("string")
       .push_back(true)
       .push_back(nullptr);
    
    serialized = arr.dump();
    ASSERT_EQ(serialized, R"([1,"string",true,null])");
}

// 测试格式化输出
TEST(JsonSerializeTest, PrettyPrint)
{
    JsonObject obj;
    obj.set("string", "value")
       .set("number", 42)
       .set("boolean", true)
       .set("null", nullptr);
    
    // 不同的缩进级别
    std::string pretty0 = obj.dump(0); // 紧凑格式
    ASSERT_EQ(pretty0, R"({"string":"value","number":42,"boolean":true,"null":null})");
    
    std::string pretty2 = obj.dump(2); // 2空格缩进
    std::string expected2 = "{\n"
                           "  \"string\": \"value\",\n"
                           "  \"number\": 42,\n"
                           "  \"boolean\": true,\n"
                           "  \"null\": null\n"
                           "}";
    ASSERT_EQ(pretty2, expected2);
    
    std::string pretty4 = obj.dump(4); // 4空格缩进
    std::string expected4 = "{\n"
                           "    \"string\": \"value\",\n"
                           "    \"number\": 42,\n"
                           "    \"boolean\": true,\n"
                           "    \"null\": null\n"
                           "}";
    ASSERT_EQ(pretty4, expected4);
}

// 测试嵌套结构序列化
TEST(JsonSerializeTest, NestedStructures)
{
    // 创建复杂的嵌套结构
    JsonObject nested;
    nested.set("name", "nested object")
          .set("values", JsonArray{1, 2, 3})
          .set("config", JsonObject{
              {"enabled", true},
              {"visible", false}
          });
    
    // 紧凑格式
    std::string compact = nested.dump();
    ASSERT_EQ(compact, R"({"name":"nested object","values":[1,2,3],"config":{"enabled":true,"visible":false}})");
    
    // 格式化输出
    std::string pretty = nested.dump(2);
    std::string expected = "{\n"
                          "  \"name\": \"nested object\",\n"
                          "  \"values\": [\n"
                          "    1,\n"
                          "    2,\n"
                          "    3\n"
                          "  ],\n"
                          "  \"config\": {\n"
                          "    \"enabled\": true,\n"
                          "    \"visible\": false\n"
                          "  }\n"
                          "}";
    ASSERT_EQ(pretty, expected);
}

// 测试特殊字符序列化
TEST(JsonSerializeTest, SpecialCharacters)
{
    // 包含特殊字符的字符串
    JsonObject special;
    special.set("escaped", "\"\\/\b\f\n\r\t");
    special.set("control", std::string("\x01\x1F", 2)); // 控制字符
    
    std::string serialized = special.dump();
    ASSERT_EQ(serialized, R"({"escaped":"\"\\\/\b\f\n\r\t","control":"\u0001\u001f"})");
    
    // 解析后再序列化，确保一致性
    Json parsed = Json::parse(serialized);
    ASSERT_FALSE(parsed.has_error());
    ASSERT_EQ(parsed.dump(), serialized);
}

// 测试数字序列化
TEST(JsonSerializeTest, NumberSerialization)
{
    JsonObject numbers;
    numbers.set("integer", 42);
    numbers.set("negative", -42);
    numbers.set("float", 3.14159);
    numbers.set("exponent", 1.23e45);
    numbers.set("zero", 0);
    
    std::string serialized = numbers.dump();
    ASSERT_EQ(serialized, R"({"integer":42,"negative":-42,"float":3.14159,"exponent":1.23e+45,"zero":0})");
    
    // 验证大数字
    JsonObject big_numbers;
    big_numbers.set("max_int", std::numeric_limits<int>::max());
    big_numbers.set("min_int", std::numeric_limits<int>::min());
    big_numbers.set("max_double", 1.7976931348623157e+308); // 接近 DBL_MAX
    
    std::string big_serialized = big_numbers.dump();
    Json parsed = Json::parse(big_serialized);
    ASSERT_FALSE(parsed.has_error());
    
    // 验证解析后的值
    ASSERT_EQ(parsed["max_int"].get<int>(), std::numeric_limits<int>::max());
    ASSERT_EQ(parsed["min_int"].get<int>(), std::numeric_limits<int>::min());
    ASSERT_NEAR(parsed["max_double"].get<double>(), 1.7976931348623157e+308, 1e+300);
}

// 测试空容器序列化
TEST(JsonSerializeTest, EmptyContainers)
{
    // 空对象
    JsonObject empty_obj;
    ASSERT_EQ(empty_obj.dump(), "{}");
    ASSERT_EQ(empty_obj.dump(2), "{}");
    
    // 空数组
    JsonArray empty_arr;
    ASSERT_EQ(empty_arr.dump(), "[]");
    ASSERT_EQ(empty_arr.dump(2), "[]");
    
    // 包含空容器的对象
    JsonObject containers;
    containers.set("empty_obj", JsonObject{});
    containers.set("empty_arr", JsonArray{});
    
    ASSERT_EQ(containers.dump(), R"({"empty_obj":{},"empty_arr":[]})");
    
    std::string pretty = containers.dump(2);
    std::string expected = "{\n"
                          "  \"empty_obj\": {},\n"
                          "  \"empty_arr\": []\n"
                          "}";
    ASSERT_EQ(pretty, expected);
}

// 测试流操作符
TEST(JsonSerializeTest, StreamOperator)
{
    JsonObject obj;
    obj.set("key", "value").set("number", 42);
    
    std::ostringstream oss;
    oss << obj;
    
    ASSERT_EQ(oss.str(), R"({"key":"value","number":42})");
}

// 测试复杂结构的序列化和解析一致性
TEST(JsonSerializeTest, SerializationParsingConsistency)
{
    // 创建复杂的JSON结构
    JsonObject complex;
    complex.set("string", "value");
    complex.set("number", 42);
    complex.set("boolean", true);
    complex.set("null", nullptr);
    complex.set("array", JsonArray{1, "string", true, nullptr});
    complex.set("object", JsonObject{
        {"nested_key", "nested_value"},
        {"nested_array", JsonArray{1, 2, 3}}
    });
    
    // 序列化
    std::string serialized = complex.dump();
    
    // 解析
    Json parsed = Json::parse(serialized);
    ASSERT_FALSE(parsed.has_error());
    
    // 再次序列化
    std::string reserialized = parsed.dump();
    
    // 验证两次序列化结果一致
    ASSERT_EQ(serialized, reserialized);
}

// 测试极端情况
TEST(JsonSerializeTest, EdgeCases)
{
    // 极长的字符串
    JsonObject long_string_obj;
    std::string long_str;
    long_str.reserve(10000);
    for (int i = 0; i < 10000; i++) {
        long_str += "a";
    }
    long_string_obj.set("long_string", long_str);
    
    std::string serialized = long_string_obj.dump();
    Json parsed = Json::parse(serialized);
    ASSERT_FALSE(parsed.has_error());
    ASSERT_EQ(parsed["long_string"].get<std::string>().length(), 10000);
    
    // 深度嵌套
    Json deep = nullptr;
    for (int i = 0; i < 100; i++) {
        JsonObject obj;
        obj.set("level", deep);
        deep = obj;
    }
    
    serialized = deep.dump();
    parsed = Json::parse(serialized);
    ASSERT_FALSE(parsed.has_error());
    
    // 大数组
    JsonArray large_array;
    for (int i = 0; i < 1000; i++) {
        large_array.push_back(i);
    }
    
    serialized = large_array.dump();
    parsed = Json::parse(serialized);
    ASSERT_FALSE(parsed.has_error());
    ASSERT_EQ(parsed.size(), 1000);
    ASSERT_EQ(parsed[999].get<int>(), 999);
}

// 测试Unicode字符序列化
TEST(JsonSerializeTest, UnicodeCharacters)
{
    JsonObject unicode;
    // 注意：这里直接使用UTF-8编码的字符串
    unicode.set("chinese", "你好，世界");
    unicode.set("emoji", "😀🌍🚀");
    unicode.set("russian", "Привет, мир");
    
    std::string serialized = unicode.dump();
    Json parsed = Json::parse(serialized);
    ASSERT_FALSE(parsed.has_error());
    
    // 验证解析后的值
    ASSERT_EQ(parsed["chinese"].get<std::string>(), "你好，世界");
    ASSERT_EQ(parsed["emoji"].get<std::string>(), "😀🌍🚀");
    ASSERT_EQ(parsed["russian"].get<std::string>(), "Привет, мир");
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 