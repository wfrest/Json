#include "Json.h"
#include "test/test_utils.h"
#include <limits>
#include <vector>
#include <map>

using namespace wfrest;

// 测试基本类型检查
TEST(JsonTypeTest, TypeChecking)
{
    // 空值
    Json null_val(nullptr);
    ASSERT_TRUE(null_val.is_null());
    ASSERT_FALSE(null_val.is_boolean());
    ASSERT_FALSE(null_val.is_number());
    ASSERT_FALSE(null_val.is_string());
    ASSERT_FALSE(null_val.is_array());
    ASSERT_FALSE(null_val.is_object());
    ASSERT_EQ(null_val.type_str(), "null");
    
    // 布尔值
    Json bool_true(true);
    ASSERT_TRUE(bool_true.is_boolean());
    ASSERT_FALSE(bool_true.is_null());
    ASSERT_FALSE(bool_true.is_number());
    ASSERT_FALSE(bool_true.is_string());
    ASSERT_FALSE(bool_true.is_array());
    ASSERT_FALSE(bool_true.is_object());
    ASSERT_EQ(bool_true.type_str(), "true");
    
    Json bool_false(false);
    ASSERT_TRUE(bool_false.is_boolean());
    ASSERT_EQ(bool_false.type_str(), "false");
    
    // 数字
    Json int_val(42);
    ASSERT_TRUE(int_val.is_number());
    ASSERT_FALSE(int_val.is_null());
    ASSERT_FALSE(int_val.is_boolean());
    ASSERT_FALSE(int_val.is_string());
    ASSERT_FALSE(int_val.is_array());
    ASSERT_FALSE(int_val.is_object());
    ASSERT_EQ(int_val.type_str(), "number");
    
    Json float_val(3.14);
    ASSERT_TRUE(float_val.is_number());
    ASSERT_EQ(float_val.type_str(), "number");
    
    // 字符串
    Json str_val("test");
    ASSERT_TRUE(str_val.is_string());
    ASSERT_FALSE(str_val.is_null());
    ASSERT_FALSE(str_val.is_boolean());
    ASSERT_FALSE(str_val.is_number());
    ASSERT_FALSE(str_val.is_array());
    ASSERT_FALSE(str_val.is_object());
    ASSERT_EQ(str_val.type_str(), "string");
    
    // 数组
    Json arr_val = JsonArray{1, 2, 3};
    ASSERT_TRUE(arr_val.is_array());
    ASSERT_FALSE(arr_val.is_null());
    ASSERT_FALSE(arr_val.is_boolean());
    ASSERT_FALSE(arr_val.is_number());
    ASSERT_FALSE(arr_val.is_string());
    ASSERT_FALSE(arr_val.is_object());
    ASSERT_EQ(arr_val.type_str(), "array");
    
    // 对象
    Json obj_val = JsonObject{{"key", "value"}};
    ASSERT_TRUE(obj_val.is_object());
    ASSERT_FALSE(obj_val.is_null());
    ASSERT_FALSE(obj_val.is_boolean());
    ASSERT_FALSE(obj_val.is_number());
    ASSERT_FALSE(obj_val.is_string());
    ASSERT_FALSE(obj_val.is_array());
    ASSERT_EQ(obj_val.type_str(), "object");
}

// 测试数字类型转换
TEST(JsonTypeTest, NumberConversions)
{
    // 整数
    Json int_val(42);
    
    // 获取为不同类型
    int int_result;
    ASSERT_TRUE(int_val.get(int_result));
    ASSERT_EQ(int_result, 42);
    
    long long_result;
    ASSERT_TRUE(int_val.get(long_result));
    ASSERT_EQ(long_result, 42L);
    
    double double_result;
    ASSERT_TRUE(int_val.get(double_result));
    EXPECT_DOUBLE_EQ(double_result, 42.0);
    
    // 浮点数
    Json float_val(3.14159);
    
    double pi;
    ASSERT_TRUE(float_val.get(pi));
    EXPECT_DOUBLE_EQ(pi, 3.14159);
    
    // 浮点数转整数（截断）
    int truncated;
    ASSERT_TRUE(float_val.get(truncated));
    ASSERT_EQ(truncated, 3);
    
    // 使用模板版本
    ASSERT_EQ(int_val.get<int>(), 42);
    ASSERT_EQ(int_val.get<long>(), 42L);
    EXPECT_DOUBLE_EQ(int_val.get<double>(), 42.0);
    
    EXPECT_DOUBLE_EQ(float_val.get<double>(), 3.14159);
    ASSERT_EQ(float_val.get<int>(), 3);
    
    // 隐式转换
    int implicit_int = int_val;
    ASSERT_EQ(implicit_int, 42);
    
    double implicit_double = float_val;
    EXPECT_DOUBLE_EQ(implicit_double, 3.14159);
}

// 测试布尔类型转换
TEST(JsonTypeTest, BooleanConversions)
{
    // 布尔值
    Json true_val(true);
    Json false_val(false);
    
    bool bool_result;
    ASSERT_TRUE(true_val.get(bool_result));
    ASSERT_TRUE(bool_result);
    
    ASSERT_TRUE(false_val.get(bool_result));
    ASSERT_FALSE(bool_result);
    
    // 使用模板版本
    ASSERT_TRUE(true_val.get<bool>());
    ASSERT_FALSE(false_val.get<bool>());
    
    // 隐式转换
    bool implicit_true = true_val;
    ASSERT_TRUE(implicit_true);
    
    bool implicit_false = false_val;
    ASSERT_FALSE(implicit_false);
    
    // 布尔值不能转换为其他类型
    int int_result;
    ASSERT_FALSE(true_val.get(int_result));
    ASSERT_TRUE(true_val.has_error());
    ASSERT_EQ(true_val.error_code(), Json::TYPE_ERROR);
    
    std::string str_result;
    ASSERT_FALSE(false_val.get(str_result));
    ASSERT_TRUE(false_val.has_error());
}

// 测试字符串类型转换
TEST(JsonTypeTest, StringConversions)
{
    // 字符串
    Json str_val("test string");
    
    std::string str_result;
    ASSERT_TRUE(str_val.get(str_result));
    ASSERT_EQ(str_result, "test string");
    
    // 使用模板版本
    ASSERT_EQ(str_val.get<std::string>(), "test string");
    
    // 隐式转换
    std::string implicit_str = str_val;
    ASSERT_EQ(implicit_str, "test string");
    
    // 字符串不能转换为其他类型
    int int_result;
    ASSERT_FALSE(str_val.get(int_result));
    ASSERT_TRUE(str_val.has_error());
    
    bool bool_result;
    ASSERT_FALSE(str_val.get(bool_result));
    ASSERT_TRUE(str_val.has_error());
    
    // 数字字符串特殊情况 - 当前实现不支持自动转换
    Json num_str("42");
    ASSERT_FALSE(num_str.get(int_result));
    ASSERT_TRUE(num_str.has_error());
}

// 测试空值类型转换
TEST(JsonTypeTest, NullConversions)
{
    // 空值
    Json null_val(nullptr);
    
    std::nullptr_t null_result;
    ASSERT_TRUE(null_val.get(null_result));
    ASSERT_EQ(null_result, nullptr);
    
    // 使用模板版本
    ASSERT_EQ(null_val.get<std::nullptr_t>(), nullptr);
    
    // 空值不能转换为其他类型
    int int_result;
    ASSERT_FALSE(null_val.get(int_result));
    ASSERT_TRUE(null_val.has_error());
    
    std::string str_result;
    ASSERT_FALSE(null_val.get(str_result));
    ASSERT_TRUE(null_val.has_error());
    
    bool bool_result;
    ASSERT_FALSE(null_val.get(bool_result));
    ASSERT_TRUE(null_val.has_error());
}

// 测试数组类型转换
TEST(JsonTypeTest, ArrayConversions)
{
    // 数组
    Json arr_val = JsonArray{1, 2, 3};
    
    JsonArray arr_result;
    ASSERT_TRUE(arr_val.get(arr_result));
    ASSERT_EQ(arr_result.size(), 3);
    ASSERT_EQ(arr_result[0].get<int>(), 1);
    
    // 使用模板版本
    JsonArray template_arr = arr_val.get<JsonArray>();
    ASSERT_EQ(template_arr.size(), 3);
    ASSERT_EQ(template_arr[1].get<int>(), 2);
    
    // 隐式转换
    JsonArray implicit_arr = arr_val;
    ASSERT_EQ(implicit_arr.size(), 3);
    ASSERT_EQ(implicit_arr[2].get<int>(), 3);
    
    // 数组不能转换为其他类型
    int int_result;
    ASSERT_FALSE(arr_val.get(int_result));
    ASSERT_TRUE(arr_val.has_error());
    
    std::string str_result;
    ASSERT_FALSE(arr_val.get(str_result));
    ASSERT_TRUE(arr_val.has_error());
}

// 测试对象类型转换
TEST(JsonTypeTest, ObjectConversions)
{
    // 对象
    Json obj_val = JsonObject{{"key1", "value1"}, {"key2", 42}};
    
    JsonObject obj_result;
    ASSERT_TRUE(obj_val.get(obj_result));
    ASSERT_EQ(obj_result.size(), 2);
    ASSERT_EQ(obj_result["key1"].get<std::string>(), "value1");
    
    // 使用模板版本
    JsonObject template_obj = obj_val.get<JsonObject>();
    ASSERT_EQ(template_obj.size(), 2);
    ASSERT_EQ(template_obj["key2"].get<int>(), 42);
    
    // 隐式转换
    JsonObject implicit_obj = obj_val;
    ASSERT_EQ(implicit_obj.size(), 2);
    ASSERT_EQ(implicit_obj["key1"].get<std::string>(), "value1");
    
    // 对象不能转换为其他类型
    int int_result;
    ASSERT_FALSE(obj_val.get(int_result));
    ASSERT_TRUE(obj_val.has_error());
    
    std::string str_result;
    ASSERT_FALSE(obj_val.get(str_result));
    ASSERT_TRUE(obj_val.has_error());
}

// 测试数字边界条件
TEST(JsonTypeTest, NumberEdgeCases)
{
    // 最大整数
    Json max_int(std::numeric_limits<int>::max());
    int max_int_result;
    ASSERT_TRUE(max_int.get(max_int_result));
    ASSERT_EQ(max_int_result, std::numeric_limits<int>::max());
    
    // 最小整数
    Json min_int(std::numeric_limits<int>::min());
    int min_int_result;
    ASSERT_TRUE(min_int.get(min_int_result));
    ASSERT_EQ(min_int_result, std::numeric_limits<int>::min());
    
    // 最大长整数
    Json max_long(std::numeric_limits<long>::max());
    long max_long_result;
    ASSERT_TRUE(max_long.get(max_long_result));
    ASSERT_EQ(max_long_result, std::numeric_limits<long>::max());
    
    // 最大双精度
    Json max_double(std::numeric_limits<double>::max());
    double max_double_result;
    ASSERT_TRUE(max_double.get(max_double_result));
    EXPECT_DOUBLE_EQ(max_double_result, std::numeric_limits<double>::max());
    
    // 最小双精度（非零）
    Json min_double(std::numeric_limits<double>::min());
    double min_double_result;
    ASSERT_TRUE(min_double.get(min_double_result));
    EXPECT_DOUBLE_EQ(min_double_result, std::numeric_limits<double>::min());
    
    // 无穷大
    Json infinity(std::numeric_limits<double>::infinity());
    double infinity_result;
    ASSERT_TRUE(infinity.get(infinity_result));
    ASSERT_TRUE(std::isinf(infinity_result));
    
    // NaN
    Json nan(std::numeric_limits<double>::quiet_NaN());
    double nan_result;
    ASSERT_TRUE(nan.get(nan_result));
    ASSERT_TRUE(std::isnan(nan_result));
    
    // 整数溢出测试
    Json big_num(std::numeric_limits<long long>::max());
    int overflow_result;
    ASSERT_FALSE(detail::safe_cast(big_num.get<double>(), overflow_result));
}

// 测试类型转换错误处理
TEST(JsonTypeTest, ConversionErrorHandling)
{
    // 尝试将数字转换为字符串
    Json num(42);
    std::string str_result;
    ASSERT_FALSE(num.get(str_result));
    ASSERT_TRUE(num.has_error());
    ASSERT_EQ(num.error_code(), Json::TYPE_ERROR);
    ASSERT_FALSE(num.last_error().empty());
    
    // 尝试将字符串转换为数字
    Json str("not a number");
    int int_result;
    ASSERT_FALSE(str.get(int_result));
    ASSERT_TRUE(str.has_error());
    ASSERT_EQ(str.error_code(), Json::TYPE_ERROR);
    ASSERT_FALSE(str.last_error().empty());
    
    // 尝试将布尔值转换为数字
    Json boolean(true);
    ASSERT_FALSE(boolean.get(int_result));
    ASSERT_TRUE(boolean.has_error());
    ASSERT_EQ(boolean.error_code(), Json::TYPE_ERROR);
    
    // 尝试将null转换为数字
    Json null_val(nullptr);
    ASSERT_FALSE(null_val.get(int_result));
    ASSERT_TRUE(null_val.has_error());
    ASSERT_EQ(null_val.error_code(), Json::TYPE_ERROR);
    
    // 清除错误并重试
    num.clear_error();
    ASSERT_FALSE(num.has_error());
    ASSERT_EQ(num.error_code(), Json::NO_ERROR);
    
    // 正确的转换应该成功
    ASSERT_TRUE(num.get(int_result));
    ASSERT_EQ(int_result, 42);
}

// 测试类型转换的一致性
TEST(JsonTypeTest, ConversionConsistency)
{
    // 创建各种类型的JSON值
    Json null_val(nullptr);
    Json bool_val(true);
    Json int_val(42);
    Json double_val(3.14159);
    Json string_val("test");
    Json array_val = JsonArray{1, 2, 3};
    Json object_val = JsonObject{{"key", "value"}};
    
    // 确保每种类型都能正确转换为自身
    std::nullptr_t null_result;
    ASSERT_TRUE(null_val.get(null_result));
    
    bool bool_result;
    ASSERT_TRUE(bool_val.get(bool_result));
    ASSERT_TRUE(bool_result);
    
    int int_result;
    ASSERT_TRUE(int_val.get(int_result));
    ASSERT_EQ(int_result, 42);
    
    double double_result;
    ASSERT_TRUE(double_val.get(double_result));
    EXPECT_DOUBLE_EQ(double_result, 3.14159);
    
    std::string string_result;
    ASSERT_TRUE(string_val.get(string_result));
    ASSERT_EQ(string_result, "test");
    
    JsonArray array_result;
    ASSERT_TRUE(array_val.get(array_result));
    ASSERT_EQ(array_result.size(), 3);
    
    JsonObject object_result;
    ASSERT_TRUE(object_val.get(object_result));
    ASSERT_EQ(object_result.size(), 1);
    
    // 测试数字之间的转换
    ASSERT_TRUE(int_val.get(double_result));
    EXPECT_DOUBLE_EQ(double_result, 42.0);
    
    ASSERT_TRUE(double_val.get(int_result));
    ASSERT_EQ(int_result, 3); // 截断
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 