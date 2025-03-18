#include "Json.h"
#include "test/test_utils.h"
#include <chrono>
#include <fstream>
#include <sstream>
#include <random>
#include <string>

using namespace wfrest;

// 辅助函数：生成随机JSON字符串
std::string generate_random_json(int depth, int width) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> type_dist(0, 5); // 0-5种类型
    static std::uniform_int_distribution<> int_dist(-1000, 1000);
    static std::uniform_real_distribution<> double_dist(-1000.0, 1000.0);
    static std::uniform_int_distribution<> bool_dist(0, 1);
    static const char* chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static std::uniform_int_distribution<> char_dist(0, 61);
    static std::uniform_int_distribution<> str_len_dist(1, 20);
    
    if (depth <= 0) {
        // 叶子节点，生成基本类型
        int type = type_dist(gen);
        switch (type) {
            case 0: // null
                return "null";
            case 1: // boolean
                return bool_dist(gen) ? "true" : "false";
            case 2: // integer
                return std::to_string(int_dist(gen));
            case 3: // double
                return std::to_string(double_dist(gen));
            case 4: // string
            {
                int len = str_len_dist(gen);
                std::string s = "\"";
                for (int i = 0; i < len; ++i) {
                    s += chars[char_dist(gen)];
                }
                s += "\"";
                return s;
            }
            default: // 默认返回数字
                return std::to_string(int_dist(gen));
        }
    } else {
        // 内部节点，生成对象或数组
        int type = type_dist(gen) % 2; // 0为对象，1为数组
        if (type == 0) {
            // 对象
            std::string result = "{";
            for (int i = 0; i < width; ++i) {
                if (i > 0) result += ",";
                std::string key = "\"key";
                key += std::to_string(i);
                key += "\"";
                result += key + ":" + generate_random_json(depth - 1, width);
            }
            result += "}";
            return result;
        } else {
            // 数组
            std::string result = "[";
            for (int i = 0; i < width; ++i) {
                if (i > 0) result += ",";
                result += generate_random_json(depth - 1, width);
            }
            result += "]";
            return result;
        }
    }
}

// 辅助函数：计时器
class Timer {
public:
    Timer() : start_time(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed_ms() {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end_time - start_time).count();
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

// 测试解析性能
TEST(JsonPerformanceTest, ParsingPerformance) {
    // 生成不同大小的JSON字符串
    std::vector<std::pair<std::string, std::string>> test_cases = {
        {"小型JSON (深度2，宽度3)", generate_random_json(2, 3)},
        {"中型JSON (深度3，宽度5)", generate_random_json(3, 5)},
        {"大型JSON (深度4，宽度8)", generate_random_json(4, 8)}
    };
    
    for (const auto& test_case : test_cases) {
        const std::string& description = test_case.first;
        const std::string& json_str = test_case.second;
        
        // 预热
        for (int i = 0; i < 5; ++i) {
            Json::parse(json_str);
        }
        
        // 计时解析
        const int iterations = 100;
        Timer timer;
        for (int i = 0; i < iterations; ++i) {
            Json parsed = Json::parse(json_str);
            ASSERT_FALSE(parsed.has_error());
        }
        
        double elapsed = timer.elapsed_ms();
        double avg_time = elapsed / iterations;
        
        std::cout << "解析 " << description << ": 平均 " << avg_time 
                  << " ms/次 (总计 " << elapsed << " ms 用于 " << iterations << " 次解析)" << std::endl;
    }
}

// 测试序列化性能
TEST(JsonPerformanceTest, SerializationPerformance) {
    // 生成不同大小的JSON对象
    std::vector<std::pair<std::string, Json>> test_cases;
    
    // 小型JSON
    Json small = Json::parse(generate_random_json(2, 3));
    test_cases.push_back({"小型JSON (深度2，宽度3)", small});
    
    // 中型JSON
    Json medium = Json::parse(generate_random_json(3, 5));
    test_cases.push_back({"中型JSON (深度3，宽度5)", medium});
    
    // 大型JSON
    Json large = Json::parse(generate_random_json(4, 8));
    test_cases.push_back({"大型JSON (深度4，宽度8)", large});
    
    for (const auto& test_case : test_cases) {
        const std::string& description = test_case.first;
        const Json& json = test_case.second;
        
        // 预热
        for (int i = 0; i < 5; ++i) {
            json.dump();
        }
        
        // 计时序列化 (紧凑格式)
        const int iterations = 100;
        Timer timer;
        for (int i = 0; i < iterations; ++i) {
            std::string serialized = json.dump();
        }
        
        double elapsed = timer.elapsed_ms();
        double avg_time = elapsed / iterations;
        
        std::cout << "序列化 " << description << " (紧凑格式): 平均 " << avg_time 
                  << " ms/次 (总计 " << elapsed << " ms 用于 " << iterations << " 次序列化)" << std::endl;
        
        // 计时序列化 (格式化输出)
        timer = Timer();
        for (int i = 0; i < iterations; ++i) {
            std::string serialized = json.dump(2);
        }
        
        elapsed = timer.elapsed_ms();
        avg_time = elapsed / iterations;
        
        std::cout << "序列化 " << description << " (格式化输出): 平均 " << avg_time 
                  << " ms/次 (总计 " << elapsed << " ms 用于 " << iterations << " 次序列化)" << std::endl;
    }
}

// 测试对象操作性能
TEST(JsonPerformanceTest, ObjectOperationsPerformance) {
    // 创建一个大型对象
    JsonObject obj;
    const int num_keys = 1000;
    
    // 测试设置键值对性能
    {
        Timer timer;
        for (int i = 0; i < num_keys; ++i) {
            obj.set("key" + std::to_string(i), i);
        }
        double elapsed = timer.elapsed_ms();
        std::cout << "设置 " << num_keys << " 个键值对: " << elapsed << " ms (" 
                  << (elapsed / num_keys) << " ms/项)" << std::endl;
    }
    
    // 测试访问键值对性能
    {
        Timer timer;
        for (int i = 0; i < num_keys; ++i) {
            int value = obj["key" + std::to_string(i)].get<int>();
            ASSERT_EQ(value, i);
        }
        double elapsed = timer.elapsed_ms();
        std::cout << "访问 " << num_keys << " 个键值对: " << elapsed << " ms (" 
                  << (elapsed / num_keys) << " ms/项)" << std::endl;
    }
    
    // 测试迭代性能
    {
        Timer timer;
        int count = 0;
        for (const auto& item : obj) {
            count++;
        }
        double elapsed = timer.elapsed_ms();
        std::cout << "迭代 " << count << " 个键值对: " << elapsed << " ms (" 
                  << (elapsed / count) << " ms/项)" << std::endl;
    }
    
    // 测试查找键性能
    {
        Timer timer;
        int found = 0;
        for (int i = 0; i < num_keys; ++i) {
            if (obj.has("key" + std::to_string(i))) {
                found++;
            }
        }
        double elapsed = timer.elapsed_ms();
        std::cout << "查找 " << num_keys << " 个键 (全部存在): " << elapsed << " ms (" 
                  << (elapsed / num_keys) << " ms/项)" << std::endl;
        ASSERT_EQ(found, num_keys);
    }
    
    // 测试查找不存在的键性能
    {
        Timer timer;
        int found = 0;
        for (int i = 0; i < num_keys; ++i) {
            if (obj.has("nonexistent" + std::to_string(i))) {
                found++;
            }
        }
        double elapsed = timer.elapsed_ms();
        std::cout << "查找 " << num_keys << " 个键 (全部不存在): " << elapsed << " ms (" 
                  << (elapsed / num_keys) << " ms/项)" << std::endl;
        ASSERT_EQ(found, 0);
    }
}

// 测试数组操作性能
TEST(JsonPerformanceTest, ArrayOperationsPerformance) {
    // 创建一个大型数组
    JsonArray arr;
    const int num_items = 10000;
    
    // 测试添加元素性能
    {
        Timer timer;
        for (int i = 0; i < num_items; ++i) {
            arr.push_back(i);
        }
        double elapsed = timer.elapsed_ms();
        std::cout << "添加 " << num_items << " 个元素到数组: " << elapsed << " ms (" 
                  << (elapsed / num_items) << " ms/项)" << std::endl;
    }
    
    // 测试访问元素性能
    {
        Timer timer;
        for (int i = 0; i < num_items; ++i) {
            int value = arr[i].get<int>();
            ASSERT_EQ(value, i);
        }
        double elapsed = timer.elapsed_ms();
        std::cout << "访问 " << num_items << " 个数组元素: " << elapsed << " ms (" 
                  << (elapsed / num_items) << " ms/项)" << std::endl;
    }
    
    // 测试迭代性能
    {
        Timer timer;
        int count = 0;
        for (const auto& item : arr) {
            count++;
        }
        double elapsed = timer.elapsed_ms();
        std::cout << "迭代 " << count << " 个数组元素: " << elapsed << " ms (" 
                  << (elapsed / count) << " ms/项)" << std::endl;
    }
}

// 测试复制和移动性能
TEST(JsonPerformanceTest, CopyAndMovePerformance) {
    // 创建不同大小的JSON对象
    std::vector<std::pair<std::string, Json>> test_cases;
    
    // 小型JSON
    Json small = Json::parse(generate_random_json(2, 3));
    test_cases.push_back({"小型JSON (深度2，宽度3)", small});
    
    // 中型JSON
    Json medium = Json::parse(generate_random_json(3, 5));
    test_cases.push_back({"中型JSON (深度3，宽度5)", medium});
    
    // 大型JSON
    Json large = Json::parse(generate_random_json(4, 8));
    test_cases.push_back({"大型JSON (深度4，宽度8)", large});
    
    for (const auto& test_case : test_cases) {
        const std::string& description = test_case.first;
        const Json& json = test_case.second;
        
        // 测试复制性能
        {
            const int iterations = 1000;
            Timer timer;
            for (int i = 0; i < iterations; ++i) {
                Json copy = json;
            }
            double elapsed = timer.elapsed_ms();
            std::cout << "复制 " << description << ": 平均 " << (elapsed / iterations) 
                      << " ms/次 (总计 " << elapsed << " ms 用于 " << iterations << " 次复制)" << std::endl;
        }
        
        // 测试移动性能
        {
            const int iterations = 1000;
            Timer timer;
            for (int i = 0; i < iterations; ++i) {
                Json temp = json;
                Json moved = std::move(temp);
            }
            double elapsed = timer.elapsed_ms();
            std::cout << "移动 " << description << ": 平均 " << (elapsed / iterations) 
                      << " ms/次 (总计 " << elapsed << " ms 用于 " << iterations << " 次移动)" << std::endl;
        }
    }
}

// 测试内存使用
TEST(JsonPerformanceTest, MemoryUsage) {
    // 注意：这个测试只是粗略估计内存使用，不是精确测量
    
    // 创建一个大型对象
    const int num_keys = 10000;
    
    // 估计对象内存使用
    {
        std::vector<JsonObject> objects;
        const int num_objects = 100;
        
        // 预先分配空间，避免vector重新分配的影响
        objects.reserve(num_objects);
        
        for (int j = 0; j < num_objects; ++j) {
            JsonObject obj;
            for (int i = 0; i < num_keys; ++i) {
                obj.set("key" + std::to_string(i), i);
            }
            objects.push_back(obj);
        }
        
        std::cout << "创建了 " << num_objects << " 个各包含 " << num_keys 
                  << " 个键值对的对象，检查内存使用情况" << std::endl;
        
        // 这里可以暂停让用户手动检查内存使用
        // 实际应用中可以使用内存分析工具
    }
    
    // 创建一个大型数组
    const int num_items = 100000;
    
    // 估计数组内存使用
    {
        std::vector<JsonArray> arrays;
        const int num_arrays = 100;
        
        // 预先分配空间，避免vector重新分配的影响
        arrays.reserve(num_arrays);
        
        for (int j = 0; j < num_arrays; ++j) {
            JsonArray arr;
            for (int i = 0; i < num_items; ++i) {
                arr.push_back(i);
            }
            arrays.push_back(arr);
        }
        
        std::cout << "创建了 " << num_arrays << " 个各包含 " << num_items 
                  << " 个元素的数组，检查内存使用情况" << std::endl;
        
        // 这里可以暂停让用户手动检查内存使用
        // 实际应用中可以使用内存分析工具
    }
}

// 测试错误处理性能
TEST(JsonPerformanceTest, ErrorHandlingPerformance) {
    // 生成一组无效的JSON字符串
    std::vector<std::string> invalid_jsons = {
        "{", // 不完整的对象
        "[", // 不完整的数组
        "{\"key\":}", // 缺少值
        "{\"key\":", // 不完整的键值对
        "[1,]", // 多余的逗号
        "{\"key\":undefined}", // 未定义的值
        "\"unclosed string", // 未闭合的字符串
        "{\"key\":01}", // 无效的数字格式
    };
    
    // 测试解析错误性能
    {
        const int iterations = 100;
        Timer timer;
        for (int i = 0; i < iterations; ++i) {
            for (const auto& invalid_json : invalid_jsons) {
                Json parsed = Json::parse(invalid_json);
                ASSERT_TRUE(parsed.has_error());
            }
        }
        double elapsed = timer.elapsed_ms();
        std::cout << "解析 " << invalid_jsons.size() << " 个无效JSON字符串 " << iterations << " 次: " 
                  << elapsed << " ms (平均 " << (elapsed / (iterations * invalid_jsons.size())) 
                  << " ms/项)" << std::endl;
    }
    
    // 测试类型错误性能
    {
        Json num(42);
        std::string str;
        
        const int iterations = 10000;
        Timer timer;
        for (int i = 0; i < iterations; ++i) {
            bool result = num.get(str);
            ASSERT_FALSE(result);
            ASSERT_TRUE(num.has_error());
            num.clear_error();
        }
        double elapsed = timer.elapsed_ms();
        std::cout << "处理 " << iterations << " 次类型错误: " << elapsed << " ms (平均 " 
                  << (elapsed / iterations) << " ms/次)" << std::endl;
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 