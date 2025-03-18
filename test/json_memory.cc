#include "Json.h"
#include "test/test_utils.h"
#include <memory>

using namespace wfrest;

// 测试基本内存管理
TEST(JsonMemoryTest, BasicMemoryManagement)
{
    // 创建一个JSON对象并确保它被正确分配
    Json* json_ptr = new Json();
    ASSERT_TRUE(json_ptr->is_null());
    
    // 设置一些值
    (*json_ptr)["key"] = "value";
    ASSERT_EQ((*json_ptr)["key"].get<std::string>(), "value");
    
    // 删除对象，确保没有内存泄漏
    delete json_ptr;
    
    // 使用智能指针
    std::unique_ptr<Json> smart_ptr = std::make_unique<Json>();
    (*smart_ptr)["key"] = "value";
    ASSERT_EQ((*smart_ptr)["key"].get<std::string>(), "value");
    
    // 智能指针会自动释放内存
}

// 测试复制构造和赋值
TEST(JsonMemoryTest, CopyConstructionAndAssignment)
{
    // 创建原始JSON对象
    Json original;
    original["string"] = "value";
    original["number"] = 42;
    original["array"] = JsonArray{1, 2, 3};
    original["object"] = JsonObject{{"nested", "value"}};
    
    // 使用复制构造函数
    Json copy_constructed(original);
    
    // 验证复制是深拷贝
    ASSERT_EQ(copy_constructed["string"].get<std::string>(), "value");
    ASSERT_EQ(copy_constructed["number"].get<int>(), 42);
    ASSERT_EQ(copy_constructed["array"][1].get<int>(), 2);
    ASSERT_EQ(copy_constructed["object"]["nested"].get<std::string>(), "value");
    
    // 修改原始对象不应影响副本
    original["string"] = "new value";
    original["number"] = 100;
    original["array"][1] = 99;
    original["object"]["nested"] = "new nested";
    
    ASSERT_EQ(copy_constructed["string"].get<std::string>(), "value"); // 不变
    ASSERT_EQ(copy_constructed["number"].get<int>(), 42); // 不变
    ASSERT_EQ(copy_constructed["array"][1].get<int>(), 2); // 不变
    ASSERT_EQ(copy_constructed["object"]["nested"].get<std::string>(), "value"); // 不变
    
    // 使用赋值运算符
    Json assigned;
    assigned = original;
    
    // 验证赋值是深拷贝
    ASSERT_EQ(assigned["string"].get<std::string>(), "new value");
    ASSERT_EQ(assigned["number"].get<int>(), 100);
    ASSERT_EQ(assigned["array"][1].get<int>(), 99);
    ASSERT_EQ(assigned["object"]["nested"].get<std::string>(), "new nested");
    
    // 修改原始对象不应影响赋值副本
    original["string"] = "another value";
    ASSERT_EQ(assigned["string"].get<std::string>(), "new value"); // 不变
    
    // 自赋值测试
    original = original;
    ASSERT_EQ(original["string"].get<std::string>(), "another value");
}

// 测试移动构造和赋值
TEST(JsonMemoryTest, MoveConstructionAndAssignment)
{
    // 创建原始JSON对象
    Json original;
    original["string"] = "value";
    original["number"] = 42;
    original["array"] = JsonArray{1, 2, 3};
    
    // 使用移动构造函数
    Json moved_constructed(std::move(original));
    
    // 原始对象应该被清空
    ASSERT_TRUE(original.is_null() || original.empty());
    
    // 移动后的对象应该包含原始数据
    ASSERT_EQ(moved_constructed["string"].get<std::string>(), "value");
    ASSERT_EQ(moved_constructed["number"].get<int>(), 42);
    ASSERT_EQ(moved_constructed["array"][1].get<int>(), 2);
    
    // 创建另一个原始对象
    Json another;
    another["key"] = "value";
    
    // 使用移动赋值运算符
    Json moved_assigned;
    moved_assigned = std::move(another);
    
    // 原始对象应该被清空
    ASSERT_TRUE(another.is_null() || another.empty());
    
    // 移动后的对象应该包含原始数据
    ASSERT_EQ(moved_assigned["key"].get<std::string>(), "value");
    
    // 自移动测试（这是未定义行为，但不应崩溃）
    Json self_move;
    self_move["key"] = "value";
    self_move = std::move(self_move);
    // 不做断言，因为行为是未定义的
}

// 测试所有权转移
TEST(JsonMemoryTest, OwnershipTransfer)
{
    // 测试对象所有权转移
    {
        JsonObject obj;
        obj.set("key", "value");
        
        Json json;
        json["object"] = obj; // 这应该转移所有权或进行深拷贝
        
        // 修改原始对象不应影响json中的对象
        obj.set("key", "new value");
        ASSERT_EQ(json["object"]["key"].get<std::string>(), "value");
    }
    
    // 测试数组所有权转移
    {
        JsonArray arr;
        arr.push_back(1).push_back(2);
        
        Json json;
        json["array"] = arr; // 这应该转移所有权或进行深拷贝
        
        // 修改原始数组不应影响json中的数组
        arr[0] = 99;
        ASSERT_EQ(json["array"][0].get<int>(), 1);
    }
    
    // 测试push_back的所有权转移
    {
        Json json = JsonArray();
        
        Json value(42);
        json.push_back(value);
        
        // 修改原始值不应影响数组中的值
        value = 99;
        ASSERT_EQ(json[0].get<int>(), 42);
    }
    
    // 测试set的所有权转移
    {
        Json json = JsonObject();
        
        Json value("string");
        json.set("key", value);
        
        // 修改原始值不应影响对象中的值
        value = "new string";
        ASSERT_EQ(json["key"].get<std::string>(), "string");
    }
}

// 测试嵌套结构的内存管理
TEST(JsonMemoryTest, NestedStructures)
{
    // 创建复杂的嵌套结构
    Json complex = JsonObject{
        {"array", JsonArray{
            1,
            JsonObject{{"key", "value"}},
            JsonArray{4, 5, 6}
        }},
        {"object", JsonObject{
            {"nested", JsonObject{
                {"deep", "value"}
            }}
        }}
    };
    
    // 复制嵌套结构
    Json copy = complex;
    
    // 修改原始结构
    complex["array"][1]["key"] = "new value";
    complex["object"]["nested"]["deep"] = "new deep";
    
    // 验证副本不受影响
    ASSERT_EQ(copy["array"][1]["key"].get<std::string>(), "value");
    ASSERT_EQ(copy["object"]["nested"]["deep"].get<std::string>(), "value");
    
    // 移动嵌套结构
    Json moved = std::move(complex);
    
    // 原始结构应该被清空
    ASSERT_TRUE(complex.is_null() || complex.empty());
    
    // 移动后的结构应该包含修改后的数据
    ASSERT_EQ(moved["array"][1]["key"].get<std::string>(), "new value");
    ASSERT_EQ(moved["object"]["nested"]["deep"].get<std::string>(), "new deep");
}

// 测试大型结构的内存管理
TEST(JsonMemoryTest, LargeStructures)
{
    // 创建大型数组
    JsonArray large_array;
    for (int i = 0; i < 1000; i++) {
        large_array.push_back(i);
    }
    
    // 复制大型数组
    JsonArray copy_array = large_array;
    
    // 验证复制是深拷贝
    for (int i = 0; i < 1000; i++) {
        ASSERT_EQ(copy_array[i].get<int>(), i);
    }
    
    // 修改原始数组
    for (int i = 0; i < 1000; i++) {
        large_array[i] = i * 2;
    }
    
    // 验证副本不受影响
    for (int i = 0; i < 1000; i++) {
        ASSERT_EQ(copy_array[i].get<int>(), i);
    }
    
    // 创建大型对象
    JsonObject large_object;
    for (int i = 0; i < 1000; i++) {
        large_object.set("key" + std::to_string(i), i);
    }
    
    // 复制大型对象
    JsonObject copy_object = large_object;
    
    // 验证复制是深拷贝
    for (int i = 0; i < 1000; i++) {
        ASSERT_EQ(copy_object["key" + std::to_string(i)].get<int>(), i);
    }
    
    // 修改原始对象
    for (int i = 0; i < 1000; i++) {
        large_object.set("key" + std::to_string(i), i * 2);
    }
    
    // 验证副本不受影响
    for (int i = 0; i < 1000; i++) {
        ASSERT_EQ(copy_object["key" + std::to_string(i)].get<int>(), i);
    }
}

// 测试临时对象和引用
TEST(JsonMemoryTest, TemporaryObjectsAndReferences)
{
    // 使用临时对象
    Json json;
    json["key"] = JsonObject{{"temp", "value"}};
    ASSERT_EQ(json["key"]["temp"].get<std::string>(), "value");
    
    // 使用临时数组
    json["array"] = JsonArray{1, 2, 3};
    ASSERT_EQ(json["array"][1].get<int>(), 2);
    
    // 引用和修改
    Json& ref = json["key"];
    ref["new_key"] = "new_value";
    ASSERT_EQ(json["key"]["new_key"].get<std::string>(), "new_value");
    
    // 常量引用
    const Json& const_ref = json["array"];
    ASSERT_EQ(const_ref[2].get<int>(), 3);
    
    // 临时对象的链式操作
    json["nested"]["deep"]["very_deep"] = "value";
    ASSERT_EQ(json["nested"]["deep"]["very_deep"].get<std::string>(), "value");
}

// 测试内存清理
TEST(JsonMemoryTest, MemoryCleanup)
{
    // 创建JSON对象
    Json json;
    json["string"] = "value";
    json["array"] = JsonArray{1, 2, 3};
    json["object"] = JsonObject{{"key", "value"}};
    
    // 清除对象
    json.clear();
    
    // 验证对象被清空但类型保持不变
    ASSERT_TRUE(json.is_object());
    ASSERT_TRUE(json.empty());
    ASSERT_EQ(json.size(), 0);
    
    // 重新添加数据
    json["new_key"] = "new_value";
    ASSERT_EQ(json["new_key"].get<std::string>(), "new_value");
    
    // 创建数组
    Json array = JsonArray{1, 2, 3};
    
    // 清除数组
    array.clear();
    
    // 验证数组被清空但类型保持不变
    ASSERT_TRUE(array.is_array());
    ASSERT_TRUE(array.empty());
    ASSERT_EQ(array.size(), 0);
    
    // 重新添加数据
    array.push_back(42);
    ASSERT_EQ(array[0].get<int>(), 42);
}

// 测试析构函数
TEST(JsonMemoryTest, Destructor)
{
    // 创建嵌套结构
    {
        Json nested = JsonObject{
            {"array", JsonArray{
                JsonObject{{"key", "value"}},
                JsonArray{1, 2, 3}
            }},
            {"object", JsonObject{
                {"nested", JsonObject{
                    {"deep", "value"}
                }}
            }}
        };
        
        // 对象将在作用域结束时被销毁
    }
    
    // 没有直接的方法测试析构函数是否正确释放内存
    // 这主要依赖于内存泄漏检测工具
    
    // 但我们可以测试多次创建和销毁对象
    for (int i = 0; i < 1000; i++) {
        Json temp = JsonObject{
            {"key", "value"},
            {"array", JsonArray{1, 2, 3}}
        };
    }
}

// 测试错误状态下的内存管理
TEST(JsonMemoryTest, ErrorStateMemoryManagement)
{
    // 创建解析错误的JSON
    Json error_json = Json::parse("{invalid}");
    ASSERT_TRUE(error_json.has_error());
    
    // 复制错误状态
    Json copy_error = error_json;
    ASSERT_TRUE(copy_error.has_error());
    ASSERT_EQ(copy_error.error_code(), error_json.error_code());
    
    // 移动错误状态
    Json moved_error = std::move(error_json);
    ASSERT_TRUE(moved_error.has_error());
    
    // 原始对象应该被清空
    ASSERT_FALSE(error_json.has_error());
    
    // 清除错误并添加数据
    moved_error.clear_error();
    moved_error["key"] = "value";
    ASSERT_FALSE(moved_error.has_error());
    ASSERT_EQ(moved_error["key"].get<std::string>(), "value");
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 