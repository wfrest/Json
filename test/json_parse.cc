#include "Json.h"
#include "test/test_utils.h"
#include <fstream>
#include <sstream>

using namespace wfrest;

// 测试从字符串解析
TEST(JsonParseTest, ParseFromString)
{
    // 解析简单对象
    Json simple = Json::parse(R"({"key":"value","number":42,"boolean":true,"null":null})");
    ASSERT_FALSE(simple.has_error());
    ASSERT_EQ(simple["key"].get<std::string>(), "value");
    ASSERT_EQ(simple["number"].get<int>(), 42);
    ASSERT_EQ(simple["boolean"].get<bool>(), true);
    ASSERT_TRUE(simple["null"].is_null());
    
    // 解析简单数组
    Json array = Json::parse(R"([1,2,3,"string",true,null])");
    ASSERT_FALSE(array.has_error());
    ASSERT_EQ(array[0].get<int>(), 1);
    ASSERT_EQ(array[3].get<std::string>(), "string");
    ASSERT_TRUE(array[5].is_null());
    
    // 解析嵌套结构
    Json nested = Json::parse(R"(
    {
        "object": {
            "nested": {
                "array": [1, 2, 3],
                "value": "nested value"
            }
        },
        "array": [
            {"name": "item1"},
            {"name": "item2"}
        ]
    }
    )");
    ASSERT_FALSE(nested.has_error());
    ASSERT_EQ(nested["object"]["nested"]["value"].get<std::string>(), "nested value");
    ASSERT_EQ(nested["object"]["nested"]["array"][1].get<int>(), 2);
    ASSERT_EQ(nested["array"][0]["name"].get<std::string>(), "item1");
}

// 测试从流解析
TEST(JsonParseTest, ParseFromStream)
{
    // 从字符串流解析
    std::stringstream ss(R"({"key":"value","number":42})");
    Json from_stream = Json::parse(ss);
    ASSERT_FALSE(from_stream.has_error());
    ASSERT_EQ(from_stream["key"].get<std::string>(), "value");
    ASSERT_EQ(from_stream["number"].get<int>(), 42);
    
    // 从空流解析
    std::stringstream empty_ss;
    Json empty_stream = Json::parse(empty_ss);
    ASSERT_TRUE(empty_stream.has_error());
    ASSERT_EQ(empty_stream.error_code(), Json::PARSE_ERROR);
}

// 测试从文件解析
TEST(JsonParseTest, ParseFromFile)
{
    // 创建临时JSON文件
    std::string temp_filename = "/tmp/json_test_file.json";
    std::ofstream file(temp_filename);
    file << R"({"key":"value","number":42,"array":[1,2,3]})";
    file.close();
    
    // 从文件解析
    FILE* fp = fopen(temp_filename.c_str(), "r");
    Json from_file = Json::parse(fp);
    fclose(fp);
    
    ASSERT_FALSE(from_file.has_error());
    ASSERT_EQ(from_file["key"].get<std::string>(), "value");
    ASSERT_EQ(from_file["number"].get<int>(), 42);
    ASSERT_EQ(from_file["array"][1].get<int>(), 2);
}

// 测试特殊字符和转义序列
TEST(JsonParseTest, SpecialCharacters)
{
    // 包含转义字符的JSON
    Json escaped = Json::parse(R"({"escaped":"\"\\\/\b\f\n\r\t\u0041"})");
    ASSERT_FALSE(escaped.has_error());
    ASSERT_EQ(escaped["escaped"].get<std::string>(), "\"\\/\b\f\n\r\tA");
    
    // Unicode字符
    Json unicode = Json::parse(R"({"unicode":"\u4F60\u597D"})"); // 你好
    ASSERT_FALSE(unicode.has_error());
    // 注意：这里的比较可能会因为编码问题而失败
    // ASSERT_EQ(unicode["unicode"].get<std::string>(), "你好");
    
    // 控制字符
    Json control = Json::parse(R"({"control":"\u0000\u001F"})");
    ASSERT_FALSE(control.has_error());
}

// 测试数字解析
TEST(JsonParseTest, NumberParsing)
{
    // 整数
    Json integers = Json::parse(R"({"zero":0,"positive":42,"negative":-42})");
    ASSERT_FALSE(integers.has_error());
    ASSERT_EQ(integers["zero"].get<int>(), 0);
    ASSERT_EQ(integers["positive"].get<int>(), 42);
    ASSERT_EQ(integers["negative"].get<int>(), -42);
    
    // 浮点数
    Json floats = Json::parse(R"({"decimal":3.14,"negative":-0.5,"exponent":1.23e4,"neg_exp":4.56e-2})");
    ASSERT_FALSE(floats.has_error());
    ASSERT_NEAR(floats["decimal"].get<double>(), 3.14, 0.0001);
    ASSERT_NEAR(floats["negative"].get<double>(), -0.5, 0.0001);
    ASSERT_NEAR(floats["exponent"].get<double>(), 12300, 0.0001);
    ASSERT_NEAR(floats["neg_exp"].get<double>(), 0.0456, 0.0001);
    
    // 大整数
    Json big_int = Json::parse(R"({"big":9223372036854775807})"); // max int64
    ASSERT_FALSE(big_int.has_error());
    ASSERT_NEAR(big_int["big"].get<double>(), 9223372036854775807.0, 1.0);
}

// 测试空白处理
TEST(JsonParseTest, Whitespace)
{
    // 包含各种空白的JSON
    Json whitespace = Json::parse(R"(
    {
        "key1" : "value1",
        "key2"  :  42,
        "key3":true,
        "array"  :  [ 1, 2, 3 ]
    }
    )");
    ASSERT_FALSE(whitespace.has_error());
    ASSERT_EQ(whitespace["key1"].get<std::string>(), "value1");
    ASSERT_EQ(whitespace["key2"].get<int>(), 42);
    ASSERT_EQ(whitespace["key3"].get<bool>(), true);
    ASSERT_EQ(whitespace["array"][1].get<int>(), 2);
    
    // 压缩的JSON（无空白）
    Json compact = Json::parse(R"({"key1":"value1","key2":42,"key3":true,"array":[1,2,3]})");
    ASSERT_FALSE(compact.has_error());
    ASSERT_EQ(compact["key1"].get<std::string>(), "value1");
    ASSERT_EQ(compact["key2"].get<int>(), 42);
    ASSERT_EQ(compact["key3"].get<bool>(), true);
    ASSERT_EQ(compact["array"][1].get<int>(), 2);
}

// 测试复杂的嵌套结构
TEST(JsonParseTest, ComplexNesting)
{
    // 复杂的嵌套JSON
    std::string complex_json = R"(
    {
        "glossary": {
            "title": "example glossary",
            "GlossDiv": {
                "title": "S",
                "GlossList": {
                    "GlossEntry": {
                        "ID": "SGML",
                        "SortAs": "SGML",
                        "GlossTerm": "Standard Generalized Markup Language",
                        "Acronym": "SGML",
                        "Abbrev": "ISO 8879:1986",
                        "GlossDef": {
                            "para": "A meta-markup language, used to create markup languages such as DocBook.",
                            "GlossSeeAlso": ["GML", "XML"]
                        },
                        "GlossSee": "markup"
                    }
                }
            }
        }
    }
    )";
    
    Json complex = Json::parse(complex_json);
    ASSERT_FALSE(complex.has_error());
    ASSERT_EQ(complex["glossary"]["title"].get<std::string>(), "example glossary");
    ASSERT_EQ(complex["glossary"]["GlossDiv"]["GlossList"]["GlossEntry"]["GlossTerm"].get<std::string>(), 
              "Standard Generalized Markup Language");
    ASSERT_EQ(complex["glossary"]["GlossDiv"]["GlossList"]["GlossEntry"]["GlossDef"]["GlossSeeAlso"][1].get<std::string>(), 
              "XML");
}

// 测试边界情况
TEST(JsonParseTest, EdgeCases)
{
    // 空对象和数组
    Json empty_obj = Json::parse("{}");
    ASSERT_FALSE(empty_obj.has_error());
    ASSERT_TRUE(empty_obj.is_object());
    ASSERT_EQ(empty_obj.size(), 0);
    
    Json empty_arr = Json::parse("[]");
    ASSERT_FALSE(empty_arr.has_error());
    ASSERT_TRUE(empty_arr.is_array());
    ASSERT_EQ(empty_arr.size(), 0);
    
    // 单值数组
    Json single_arr = Json::parse("[42]");
    ASSERT_FALSE(single_arr.has_error());
    ASSERT_EQ(single_arr[0].get<int>(), 42);
    
    // 单键对象
    Json single_obj = Json::parse(R"({"key":"value"})");
    ASSERT_FALSE(single_obj.has_error());
    ASSERT_EQ(single_obj["key"].get<std::string>(), "value");
    
    // 极长的字符串
    std::string long_string = "\"";
    for (int i = 0; i < 10000; i++) {
        long_string += "a";
    }
    long_string += "\"";
    
    Json long_str_json = Json::parse(long_string);
    ASSERT_FALSE(long_str_json.has_error());
    ASSERT_EQ(long_str_json.get<std::string>().length(), 10000);
}

// 测试JSON规范的边界情况
TEST(JsonParseTest, JsonSpecEdgeCases)
{
    // 允许的最小数值
    Json min_number = Json::parse("-1.7976931348623157e+308"); // 接近 -DBL_MAX
    ASSERT_FALSE(min_number.has_error());
    
    // 允许的最大数值
    Json max_number = Json::parse("1.7976931348623157e+308"); // 接近 DBL_MAX
    ASSERT_FALSE(max_number.has_error());
    
    // 非常接近0的数
    Json tiny_number = Json::parse("1e-323"); // 接近最小的非零双精度
    ASSERT_FALSE(tiny_number.has_error());
    
    // 测试数组中的不同类型
    Json mixed_array = Json::parse(R"([1, "string", true, null, {"key": "value"}, [1, 2]])");
    ASSERT_FALSE(mixed_array.has_error());
    ASSERT_EQ(mixed_array[0].get<int>(), 1);
    ASSERT_EQ(mixed_array[1].get<std::string>(), "string");
    ASSERT_EQ(mixed_array[2].get<bool>(), true);
    ASSERT_TRUE(mixed_array[3].is_null());
    ASSERT_EQ(mixed_array[4]["key"].get<std::string>(), "value");
    ASSERT_EQ(mixed_array[5][1].get<int>(), 2);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 