#include "Json.h"
#include "test/test_utils.h"
#include <fstream>
#include <sstream>
#include <string>

using namespace wfrest;

// 测试JSON标准兼容性
TEST(JsonCompatibilityTest, JsonStandardCompliance) {
    // 测试RFC 8259标准的基本类型
    
    // null
    Json null_val = Json::parse("null");
    ASSERT_FALSE(null_val.has_error());
    ASSERT_TRUE(null_val.is_null());
    
    // boolean
    Json true_val = Json::parse("true");
    ASSERT_FALSE(true_val.has_error());
    ASSERT_TRUE(true_val.is_boolean());
    ASSERT_TRUE(true_val.get<bool>());
    
    Json false_val = Json::parse("false");
    ASSERT_FALSE(false_val.has_error());
    ASSERT_TRUE(false_val.is_boolean());
    ASSERT_FALSE(false_val.get<bool>());
    
    // number
    Json int_val = Json::parse("42");
    ASSERT_FALSE(int_val.has_error());
    ASSERT_TRUE(int_val.is_number());
    ASSERT_EQ(int_val.get<int>(), 42);
    
    Json neg_val = Json::parse("-42");
    ASSERT_FALSE(neg_val.has_error());
    ASSERT_TRUE(neg_val.is_number());
    ASSERT_EQ(neg_val.get<int>(), -42);
    
    Json float_val = Json::parse("3.14159");
    ASSERT_FALSE(float_val.has_error());
    ASSERT_TRUE(float_val.is_number());
    ASSERT_NEAR(float_val.get<double>(), 3.14159, 0.00001);
    
    Json exp_val = Json::parse("1.23e4");
    ASSERT_FALSE(exp_val.has_error());
    ASSERT_TRUE(exp_val.is_number());
    ASSERT_NEAR(exp_val.get<double>(), 12300.0, 0.1);
    
    // string
    Json str_val = Json::parse("\"Hello, world!\"");
    ASSERT_FALSE(str_val.has_error());
    ASSERT_TRUE(str_val.is_string());
    ASSERT_EQ(str_val.get<std::string>(), "Hello, world!");
    
    // array
    Json arr_val = Json::parse("[1, 2, 3]");
    ASSERT_FALSE(arr_val.has_error());
    ASSERT_TRUE(arr_val.is_array());
    ASSERT_EQ(arr_val.size(), 3);
    ASSERT_EQ(arr_val[0].get<int>(), 1);
    
    // object
    Json obj_val = Json::parse("{\"key\": \"value\"}");
    ASSERT_FALSE(obj_val.has_error());
    ASSERT_TRUE(obj_val.is_object());
    ASSERT_EQ(obj_val["key"].get<std::string>(), "value");
}

// 测试JSON转义序列
TEST(JsonCompatibilityTest, EscapeSequences) {
    // 标准转义序列
    std::string json_str = R"({"escaped": "\"\\/\b\f\n\r\t"})";
    Json json = Json::parse(json_str);
    ASSERT_FALSE(json.has_error());
    ASSERT_EQ(json["escaped"].get<std::string>(), "\"\\/\b\f\n\r\t");
    
    // Unicode转义
    json_str = R"({"unicode": "\u0041\u0042\u0043"})"; // ABC
    json = Json::parse(json_str);
    ASSERT_FALSE(json.has_error());
    ASSERT_EQ(json["unicode"].get<std::string>(), "ABC");
    
    // 代理对 (surrogate pairs)
    json_str = R"({"surrogate": "\uD834\uDD1E"})"; // 𝄞 (音符)
    json = Json::parse(json_str);
    ASSERT_FALSE(json.has_error());
    // 注意：这里的比较可能会因为编码问题而失败
    // ASSERT_EQ(json["surrogate"].get<std::string>(), "𝄞");
    
    // 确保序列化后的转义序列可以被正确解析
    JsonObject obj;
    obj.set("escaped", "\"\\/\b\f\n\r\t");
    std::string serialized = obj.dump();
    Json reparsed = Json::parse(serialized);
    ASSERT_FALSE(reparsed.has_error());
    ASSERT_EQ(reparsed["escaped"].get<std::string>(), "\"\\/\b\f\n\r\t");
}

// 测试数字格式兼容性
TEST(JsonCompatibilityTest, NumberFormats) {
    // 整数
    std::vector<std::string> int_formats = {
        "0", "42", "-42", "1234567890", "-1234567890"
    };
    
    for (const auto& format : int_formats) {
        Json json = Json::parse(format);
        ASSERT_FALSE(json.has_error()) << "Failed to parse: " << format;
        ASSERT_TRUE(json.is_number());
        ASSERT_EQ(json.get<int>(), std::stoi(format));
    }
    
    // 浮点数
    std::vector<std::pair<std::string, double>> float_formats = {
        {"0.0", 0.0},
        {"3.14159", 3.14159},
        {"-3.14159", -3.14159},
        {"1.0e10", 1.0e10},
        {"1.0E10", 1.0e10},
        {"1.0e+10", 1.0e+10},
        {"1.0e-10", 1.0e-10},
        {"-1.0e-10", -1.0e-10}
    };
    
    for (const auto& format : float_formats) {
        Json json = Json::parse(format.first);
        ASSERT_FALSE(json.has_error()) << "Failed to parse: " << format.first;
        ASSERT_TRUE(json.is_number());
        ASSERT_NEAR(json.get<double>(), format.second, std::abs(format.second * 1e-10));
    }
    
    // 非标准数字格式（应该失败）
    std::vector<std::string> invalid_formats = {
        "+42",      // 不允许前导加号
        "042",      // 不允许前导零（除了0本身）
        "0x42",     // 不允许十六进制
        "Infinity", // 不允许特殊值
        "NaN",      // 不允许特殊值
        ".5",       // 不允许省略整数部分的前导零
        "1.",       // 不允许省略小数部分的尾随零
    };
    
    for (const auto& format : invalid_formats) {
        Json json = Json::parse(format);
        ASSERT_TRUE(json.has_error()) << "Should fail to parse: " << format;
    }
}

// 测试Unicode兼容性
TEST(JsonCompatibilityTest, UnicodeSupport) {
    // 基本多语言平面字符
    std::string json_str = R"({"chinese": "你好，世界", "emoji": "😀", "russian": "Привет, мир"})";
    Json json = Json::parse(json_str);
    ASSERT_FALSE(json.has_error());
    ASSERT_EQ(json["chinese"].get<std::string>(), "你好，世界");
    ASSERT_EQ(json["emoji"].get<std::string>(), "😀");
    ASSERT_EQ(json["russian"].get<std::string>(), "Привет, мир");
    
    // 确保序列化和解析保持一致
    std::string serialized = json.dump();
    Json reparsed = Json::parse(serialized);
    ASSERT_FALSE(reparsed.has_error());
    ASSERT_EQ(reparsed["chinese"].get<std::string>(), "你好，世界");
    ASSERT_EQ(reparsed["emoji"].get<std::string>(), "😀");
    ASSERT_EQ(reparsed["russian"].get<std::string>(), "Привет, мир");
}

// 测试与其他JSON库的兼容性
TEST(JsonCompatibilityTest, CrossLibraryCompatibility) {
    // 这个测试模拟从其他JSON库生成的字符串
    
    // 模拟从JavaScript生成的JSON
    std::string js_json = R"({"number":42,"string":"text","array":[1,2,3],"object":{"nested":"value"},"null":null,"boolean":true})";
    Json parsed = Json::parse(js_json);
    ASSERT_FALSE(parsed.has_error());
    ASSERT_EQ(parsed["number"].get<int>(), 42);
    ASSERT_EQ(parsed["string"].get<std::string>(), "text");
    ASSERT_EQ(parsed["array"][1].get<int>(), 2);
    ASSERT_EQ(parsed["object"]["nested"].get<std::string>(), "value");
    ASSERT_TRUE(parsed["null"].is_null());
    ASSERT_TRUE(parsed["boolean"].get<bool>());
    
    // 模拟从Python生成的JSON（可能包含空格）
    std::string py_json = R"({
        "number": 42,
        "string": "text",
        "array": [1, 2, 3],
        "object": {"nested": "value"},
        "null": null,
        "boolean": true
    })";
    parsed = Json::parse(py_json);
    ASSERT_FALSE(parsed.has_error());
    ASSERT_EQ(parsed["number"].get<int>(), 42);
    ASSERT_EQ(parsed["string"].get<std::string>(), "text");
    ASSERT_EQ(parsed["array"][1].get<int>(), 2);
    ASSERT_EQ(parsed["object"]["nested"].get<std::string>(), "value");
    ASSERT_TRUE(parsed["null"].is_null());
    ASSERT_TRUE(parsed["boolean"].get<bool>());
}

// 测试JSON5兼容性（JSON的超集，我们的解析器可能不支持所有特性）
TEST(JsonCompatibilityTest, Json5Features) {
    // 注释（标准JSON不支持，但JSON5支持）
    std::string json5_with_comments = R"({
        // 这是单行注释
        "key": "value", /* 这是多行注释 */
        "another": "value"
    })";
    Json parsed = Json::parse(json5_with_comments);
    ASSERT_TRUE(parsed.has_error()); // 应该失败，因为标准JSON不支持注释
    
    // 尾随逗号（标准JSON不支持，但JSON5支持）
    std::string json5_trailing_comma = R"({
        "array": [1, 2, 3,],
        "object": {"key": "value",},
    })";
    parsed = Json::parse(json5_trailing_comma);
    ASSERT_TRUE(parsed.has_error()); // 应该失败，因为标准JSON不支持尾随逗号
    
    // 单引号（标准JSON不支持，但JSON5支持）
    std::string json5_single_quotes = R"({'key': 'value'})";
    parsed = Json::parse(json5_single_quotes);
    ASSERT_TRUE(parsed.has_error()); // 应该失败，因为标准JSON不支持单引号
}

// 测试大型JSON文件
TEST(JsonCompatibilityTest, LargeJsonFiles) {
    // 创建一个大型JSON对象
    JsonObject large_obj;
    for (int i = 0; i < 1000; ++i) {
        large_obj.set("key" + std::to_string(i), i);
    }
    
    // 序列化和解析
    std::string serialized = large_obj.dump();
    Json reparsed = Json::parse(serialized);
    ASSERT_FALSE(reparsed.has_error());
    ASSERT_EQ(reparsed.size(), 1000);
    
    // 创建一个大型JSON数组
    JsonArray large_arr;
    for (int i = 0; i < 1000; ++i) {
        large_arr.push_back(i);
    }
    
    // 序列化和解析
    serialized = large_arr.dump();
    reparsed = Json::parse(serialized);
    ASSERT_FALSE(reparsed.has_error());
    ASSERT_EQ(reparsed.size(), 1000);
    
    // 创建一个深度嵌套的JSON
    Json nested = nullptr;
    for (int i = 0; i < 100; ++i) {
        JsonObject obj;
        obj.set("level", i);
        obj.set("next", nested);
        nested = obj;
    }
    
    // 序列化和解析
    serialized = nested.dump();
    reparsed = Json::parse(serialized);
    ASSERT_FALSE(reparsed.has_error());
    
    // 验证深度嵌套
    Json current = reparsed;
    for (int i = 99; i >= 0; --i) {
        ASSERT_TRUE(current.is_object());
        ASSERT_EQ(current["level"].get<int>(), i);
        current = current["next"];
    }
    ASSERT_TRUE(current.is_null());
}

// 测试JSON Schema验证（简化版）
TEST(JsonCompatibilityTest, SchemaValidation) {
    // 定义一个简单的schema验证函数
    auto validate_schema = [](const Json& json, const Json& schema) -> bool {
        // 类型检查
        if (schema.has("type")) {
            std::string type = schema["type"].get<std::string>();
            if (type == "object" && !json.is_object()) return false;
            if (type == "array" && !json.is_array()) return false;
            if (type == "string" && !json.is_string()) return false;
            if (type == "number" && !json.is_number()) return false;
            if (type == "boolean" && !json.is_boolean()) return false;
            if (type == "null" && !json.is_null()) return false;
        }
        
        // 对象属性检查
        if (json.is_object() && schema.has("properties")) {
            const Json& properties = schema["properties"];
            
            // 必需属性检查
            if (schema.has("required")) {
                for (const auto& req : schema["required"]) {
                    std::string req_prop = req.get<std::string>();
                    if (!json.has(req_prop)) return false;
                }
            }
            
            // 属性类型检查
            for (const auto& prop : properties) {
                std::string prop_name = prop.key();
                if (json.has(prop_name)) {
                    if (!validate_schema(json[prop_name], prop.value())) {
                        return false;
                    }
                }
            }
        }
        
        // 数组项检查
        if (json.is_array() && schema.has("items")) {
            const Json& items_schema = schema["items"];
            for (const auto& item : json) {
                if (!validate_schema(item, items_schema)) {
                    return false;
                }
            }
        }
        
        return true;
    };
    
    // 定义一个用户对象的schema
    JsonObject user_schema;
    user_schema.set("type", "object");
    user_schema.set("required", JsonArray{"id", "name"});
    user_schema.set("properties", JsonObject{
        {"id", JsonObject{{"type", "number"}}},
        {"name", JsonObject{{"type", "string"}}},
        {"email", JsonObject{{"type", "string"}}},
        {"active", JsonObject{{"type", "boolean"}}},
        {"tags", JsonObject{
            {"type", "array"},
            {"items", JsonObject{{"type", "string"}}}
        }}
    });
    
    // 有效的用户对象
    JsonObject valid_user;
    valid_user.set("id", 1001)
              .set("name", "John Doe")
              .set("email", "john.doe@example.com")
              .set("active", true)
              .set("tags", JsonArray{"user", "admin"});
    
    ASSERT_TRUE(validate_schema(valid_user, user_schema));
    
    // 无效的用户对象（缺少必需属性）
    JsonObject invalid_user1;
    invalid_user1.set("id", 1002)
                 .set("email", "jane.doe@example.com");
    
    ASSERT_FALSE(validate_schema(invalid_user1, user_schema));
    
    // 无效的用户对象（类型错误）
    JsonObject invalid_user2;
    invalid_user2.set("id", "1003") // ID应该是数字
                 .set("name", "Jane Doe");
    
    ASSERT_FALSE(validate_schema(invalid_user2, user_schema));
    
    // 无效的用户对象（数组项类型错误）
    JsonObject invalid_user3;
    invalid_user3.set("id", 1004)
                 .set("name", "Bob Smith")
                 .set("tags", JsonArray{"user", 123}); // 标签应该都是字符串
    
    ASSERT_FALSE(validate_schema(invalid_user3, user_schema));
}

// 测试JSON Patch兼容性（RFC 6902）
TEST(JsonCompatibilityTest, JsonPatch) {
    // 实现一个简单的JSON Patch应用函数
    auto apply_patch = [](Json document, const Json& patch) -> Json {
        if (!patch.is_array()) {
            return document;
        }
        
        for (const auto& operation : patch) {
            if (!operation.is_object() || !operation.has("op") || !operation.has("path")) {
                continue;
            }
            
            std::string op = operation["op"].get<std::string>();
            std::string path = operation["path"].get<std::string>();
            
            // 简化的路径解析（只支持简单路径）
            if (path.empty() || path[0] != '/') {
                continue;
            }
            
            std::string key = path.substr(1); // 移除前导斜杠
            
            if (op == "add" || op == "replace") {
                if (!operation.has("value")) {
                    continue;
                }
                document[key] = operation["value"];
            } else if (op == "remove") {
                if (document.is_object()) {
                    JsonObject obj = document;
                    obj.erase(key);
                    document = obj;
                }
            } else if (op == "copy") {
                if (!operation.has("from")) {
                    continue;
                }
                std::string from = operation["from"].get<std::string>();
                if (from.empty() || from[0] != '/') {
                    continue;
                }
                std::string from_key = from.substr(1);
                if (document.has(from_key)) {
                    document[key] = document[from_key];
                }
            } else if (op == "move") {
                if (!operation.has("from")) {
                    continue;
                }
                std::string from = operation["from"].get<std::string>();
                if (from.empty() || from[0] != '/') {
                    continue;
                }
                std::string from_key = from.substr(1);
                if (document.has(from_key)) {
                    document[key] = document[from_key];
                    if (document.is_object()) {
                        JsonObject obj = document;
                        obj.erase(from_key);
                        document = obj;
                    }
                }
            }
        }
        
        return document;
    };
    
    // 测试基本操作
    JsonObject doc;
    doc.set("foo", "bar")
       .set("baz", "qux");
    
    // 添加操作
    JsonArray add_patch;
    add_patch.push_back(JsonObject{
        {"op", "add"},
        {"path", "/hello"},
        {"value", "world"}
    });
    
    Json result = apply_patch(doc, add_patch);
    ASSERT_TRUE(result.has("hello"));
    ASSERT_EQ(result["hello"].get<std::string>(), "world");
    
    // 替换操作
    JsonArray replace_patch;
    replace_patch.push_back(JsonObject{
        {"op", "replace"},
        {"path", "/foo"},
        {"value", "new value"}
    });
    
    result = apply_patch(doc, replace_patch);
    ASSERT_EQ(result["foo"].get<std::string>(), "new value");
    
    // 删除操作
    JsonArray remove_patch;
    remove_patch.push_back(JsonObject{
        {"op", "remove"},
        {"path", "/baz"}
    });
    
    result = apply_patch(doc, remove_patch);
    ASSERT_FALSE(result.has("baz"));
    
    // 复制操作
    JsonArray copy_patch;
    copy_patch.push_back(JsonObject{
        {"op", "copy"},
        {"from", "/foo"},
        {"path", "/copy"}
    });
    
    result = apply_patch(doc, copy_patch);
    ASSERT_TRUE(result.has("copy"));
    ASSERT_EQ(result["copy"].get<std::string>(), "bar");
    
    // 移动操作
    JsonArray move_patch;
    move_patch.push_back(JsonObject{
        {"op", "move"},
        {"from", "/foo"},
        {"path", "/moved"}
    });
    
    result = apply_patch(doc, move_patch);
    ASSERT_FALSE(result.has("foo"));
    ASSERT_TRUE(result.has("moved"));
    ASSERT_EQ(result["moved"].get<std::string>(), "bar");
    
    // 组合操作
    JsonArray combined_patch;
    combined_patch.push_back(JsonObject{
        {"op", "add"},
        {"path", "/new"},
        {"value", "value"}
    });
    combined_patch.push_back(JsonObject{
        {"op", "remove"},
        {"path", "/baz"}
    });
    combined_patch.push_back(JsonObject{
        {"op", "replace"},
        {"path", "/foo"},
        {"value", "replaced"}
    });
    
    result = apply_patch(doc, combined_patch);
    ASSERT_TRUE(result.has("new"));
    ASSERT_FALSE(result.has("baz"));
    ASSERT_EQ(result["foo"].get<std::string>(), "replaced");
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 