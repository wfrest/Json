#include "Json.h"
#include "test/test_utils.h"

using namespace wfrest;

TEST(IterTest, object)
{
    Json data;
    data["key1"] = 1;
    data["key2"] = 2.0;
    data["key3"] = true;
    Json::iterator it = data.begin();
    ASSERT_EQ(it->key(), "key1");
    ASSERT_EQ(it->value().get<int>(), 1);
    ASSERT_EQ((*it).get<int>(), 1);
    it++;
    ASSERT_EQ(it->key(), "key2");
    ASSERT_EQ(it->value().get<double>(), 2.0);
    ASSERT_EQ((*it).get<double>(), 2.0);
    ++it;
    ASSERT_EQ(it->key(), "key3");
    ASSERT_EQ(it->value().get<bool>(), true);
    ASSERT_EQ((*it).get<bool>(), true);
    ++it;
    ASSERT_EQ(it, data.end());
    ++it;
    ++it; // safe to ++ forever, always stay at end() position
    ASSERT_EQ(it, data.end());
    // for (Json::iterator it = data.begin(); it != data.end(); it++)
    // {
    //     std::cout << it->key() << it->value() << std::endl;
    // }
    // for (auto it = data.begin(); it != data.end(); it++)
    // {
    //     std::cout << *it << std::endl;
    // }
    // for (const auto& it : data)
    // {
    //     std::cout << it.key() << " : " << it.value() << std::endl;
    // }
}

TEST(IterTest, object_reverse)
{
    Json data;
    data["key1"] = 1;
    data["key2"] = 2.0;
    data["key3"] = true;
    Json::reverse_iterator it = data.rbegin();
    // reverse order
    ASSERT_EQ(it->key(), "key3");
    ASSERT_EQ(it->value().get<bool>(), true);
    ASSERT_EQ((*it).get<bool>(), true);
    it++;
    ASSERT_EQ(it->key(), "key2");
    ASSERT_EQ(it->value().get<double>(), 2.0);
    ASSERT_EQ((*it).get<double>(), 2.0);
    ++it;
    ASSERT_EQ(it->key(), "key1");
    ASSERT_EQ(it->value().get<int>(), 1);
    ASSERT_EQ((*it).get<int>(), 1);
    ++it;
    ASSERT_EQ(it, data.rend());
    ++it;
    ++it; // safe to ++ forever, always stay at rend() position
    ASSERT_EQ(it, data.rend());
}

TEST(IterTest, array)
{
    Json data;
    data.push_back(1);
    data.push_back(2.0);
    data.push_back(false);
    Json::iterator it = data.begin();
    ASSERT_EQ(it->value().get<int>(), 1);
    ASSERT_EQ((*it).get<int>(), 1);
    ++it;
    ASSERT_EQ(it->value().get<double>(), 2.0);
    ASSERT_EQ((*it).get<double>(), 2.0);
    it++;
    ASSERT_EQ(it->value().get<bool>(), false);
    ASSERT_EQ((*it).get<bool>(), false);
    ++it;
    it++;
    ASSERT_EQ(it, data.end());
    // for (const auto& it : data)
    // {
    //     std::cout << it << std::endl;
    // }
}

TEST(IterTest, array_reverse)
{
    Json data;
    data.push_back(1);
    data.push_back(2.0);
    data.push_back(false);
    Json::reverse_iterator it = data.rbegin();
    ASSERT_EQ(it->value().get<bool>(), false);
    ASSERT_EQ((*it).get<bool>(), false);
    ++it;
    ASSERT_EQ(it->value().get<double>(), 2.0);
    ASSERT_EQ((*it).get<double>(), 2.0);
    it++;
    ASSERT_EQ(it->value().get<int>(), 1);
    ASSERT_EQ((*it).get<int>(), 1);
    ++it;
    it++;
    ASSERT_EQ(it, data.rend());
}

// 测试空容器迭代
TEST(IterTest, EmptyContainers)
{
    // 空对象迭代
    JsonObject empty_obj;
    ASSERT_EQ(empty_obj.begin(), empty_obj.end());
    
    // 确保可以安全地使用范围for循环
    int count = 0;
    for (const auto& item : empty_obj) {
        count++;
    }
    ASSERT_EQ(count, 0);
    
    // 空数组迭代
    JsonArray empty_arr;
    ASSERT_EQ(empty_arr.begin(), empty_arr.end());
    
    count = 0;
    for (const auto& item : empty_arr) {
        count++;
    }
    ASSERT_EQ(count, 0);
    
    // 反向迭代器
    ASSERT_EQ(empty_obj.rbegin(), empty_obj.rend());
    ASSERT_EQ(empty_arr.rbegin(), empty_arr.rend());
}

// 测试范围for循环
TEST(IterTest, RangeBasedForLoop)
{
    // 对象的范围for循环
    JsonObject obj;
    obj.set("key1", 1)
       .set("key2", "value")
       .set("key3", true);
    
    std::vector<std::string> keys;
    std::vector<std::string> types;
    
    for (const auto& item : obj) {
        keys.push_back(item.key());
        types.push_back(item.value().type_str());
    }
    
    ASSERT_EQ(keys.size(), 3);
    ASSERT_TRUE(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    ASSERT_TRUE(std::find(keys.begin(), keys.end(), "key2") != keys.end());
    ASSERT_TRUE(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    
    // 数组的范围for循环
    JsonArray arr;
    arr.push_back(1)
       .push_back("string")
       .push_back(true);
    
    std::vector<std::string> arr_types;
    
    for (const auto& item : arr) {
        arr_types.push_back(item.type_str());
    }
    
    ASSERT_EQ(arr_types.size(), 3);
    ASSERT_EQ(arr_types[0], "number");
    ASSERT_EQ(arr_types[1], "string");
    ASSERT_EQ(arr_types[2], "true");
}

// 测试迭代器修改值
TEST(IterTest, IteratorModification)
{
    // 通过迭代器修改对象值
    JsonObject obj;
    obj.set("key1", 1)
       .set("key2", 2);
    
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        it->value() = 42;
    }
    
    ASSERT_EQ(obj["key1"].get<int>(), 42);
    ASSERT_EQ(obj["key2"].get<int>(), 42);
    
    // 通过迭代器修改数组值
    JsonArray arr;
    arr.push_back(1)
       .push_back(2)
       .push_back(3);
    
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        it->value() = "modified";
    }
    
    ASSERT_EQ(arr[0].get<std::string>(), "modified");
    ASSERT_EQ(arr[1].get<std::string>(), "modified");
    ASSERT_EQ(arr[2].get<std::string>(), "modified");
}

// 测试迭代器比较操作
TEST(IterTest, IteratorComparison)
{
    JsonObject obj;
    obj.set("key1", 1)
       .set("key2", 2);
    
    auto it1 = obj.begin();
    auto it2 = obj.begin();
    auto end = obj.end();
    
    ASSERT_TRUE(it1 == it2);
    ASSERT_FALSE(it1 != it2);
    ASSERT_FALSE(it1 == end);
    ASSERT_TRUE(it1 != end);
    
    ++it1;
    ASSERT_FALSE(it1 == it2);
    ASSERT_TRUE(it1 != it2);
    
    ++it1;
    ASSERT_TRUE(it1 == end);
    
    // 数组迭代器比较
    JsonArray arr;
    arr.push_back(1)
       .push_back(2);
    
    auto arr_it1 = arr.begin();
    auto arr_it2 = arr.begin();
    auto arr_end = arr.end();
    
    ASSERT_TRUE(arr_it1 == arr_it2);
    ASSERT_FALSE(arr_it1 != arr_it2);
    ASSERT_FALSE(arr_it1 == arr_end);
    ASSERT_TRUE(arr_it1 != arr_end);
}

// Add the main function for our test framework
MAIN_TEST()
