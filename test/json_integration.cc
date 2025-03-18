#include "Json.h"
#include "test/test_utils.h"
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

using namespace wfrest;

// 模拟HTTP请求/响应类
class MockHttpResponse {
public:
    void set_status(int code) { status_code = code; }
    void set_header(const std::string& key, const std::string& value) { headers[key] = value; }
    void set_body(const std::string& content) { body = content; }
    
    int get_status() const { return status_code; }
    std::string get_header(const std::string& key) const { 
        auto it = headers.find(key);
        return it != headers.end() ? it->second : "";
    }
    std::string get_body() const { return body; }
    
private:
    int status_code = 200;
    std::map<std::string, std::string> headers;
    std::string body;
};

class MockHttpRequest {
public:
    void set_method(const std::string& m) { method = m; }
    void set_path(const std::string& p) { path = p; }
    void set_header(const std::string& key, const std::string& value) { headers[key] = value; }
    void set_body(const std::string& content) { body = content; }
    
    std::string get_method() const { return method; }
    std::string get_path() const { return path; }
    std::string get_header(const std::string& key) const { 
        auto it = headers.find(key);
        return it != headers.end() ? it->second : "";
    }
    std::string get_body() const { return body; }
    
private:
    std::string method = "GET";
    std::string path = "/";
    std::map<std::string, std::string> headers;
    std::string body;
};

// 测试JSON与HTTP响应的集成
TEST(JsonIntegrationTest, HttpResponseIntegration) {
    // 创建一个JSON对象
    JsonObject user_data;
    user_data.set("id", 1001)
             .set("name", "John Doe")
             .set("email", "john.doe@example.com")
             .set("active", true)
             .set("roles", JsonArray{"user", "admin"});
    
    // 创建HTTP响应
    MockHttpResponse response;
    
    // 设置JSON响应
    response.set_status(200);
    response.set_header("Content-Type", "application/json");
    response.set_body(user_data.dump());
    
    // 验证响应
    ASSERT_EQ(response.get_status(), 200);
    ASSERT_EQ(response.get_header("Content-Type"), "application/json");
    
    // 解析响应体
    Json parsed = Json::parse(response.get_body());
    ASSERT_FALSE(parsed.has_error());
    ASSERT_EQ(parsed["id"].get<int>(), 1001);
    ASSERT_EQ(parsed["name"].get<std::string>(), "John Doe");
    ASSERT_EQ(parsed["email"].get<std::string>(), "john.doe@example.com");
    ASSERT_EQ(parsed["active"].get<bool>(), true);
    ASSERT_EQ(parsed["roles"][0].get<std::string>(), "user");
    ASSERT_EQ(parsed["roles"][1].get<std::string>(), "admin");
    
    // 测试格式化输出
    response.set_body(user_data.dump(2));
    Json pretty_parsed = Json::parse(response.get_body());
    ASSERT_FALSE(pretty_parsed.has_error());
    ASSERT_EQ(pretty_parsed["id"].get<int>(), 1001);
}

// 测试JSON与HTTP请求的集成
TEST(JsonIntegrationTest, HttpRequestIntegration) {
    // 创建一个JSON请求体
    JsonObject request_data;
    request_data.set("action", "create")
                .set("data", JsonObject{
                    {"name", "New Product"},
                    {"price", 29.99},
                    {"tags", JsonArray{"new", "featured"}}
                });
    
    // 创建HTTP请求
    MockHttpRequest request;
    request.set_method("POST");
    request.set_path("/api/products");
    request.set_header("Content-Type", "application/json");
    request.set_body(request_data.dump());
    
    // 验证请求
    ASSERT_EQ(request.get_method(), "POST");
    ASSERT_EQ(request.get_path(), "/api/products");
    ASSERT_EQ(request.get_header("Content-Type"), "application/json");
    
    // 解析请求体
    Json parsed = Json::parse(request.get_body());
    ASSERT_FALSE(parsed.has_error());
    ASSERT_EQ(parsed["action"].get<std::string>(), "create");
    ASSERT_EQ(parsed["data"]["name"].get<std::string>(), "New Product");
    ASSERT_NEAR(parsed["data"]["price"].get<double>(), 29.99, 0.001);
    ASSERT_EQ(parsed["data"]["tags"][0].get<std::string>(), "new");
    ASSERT_EQ(parsed["data"]["tags"][1].get<std::string>(), "featured");
}

// 测试JSON与文件系统的集成
TEST(JsonIntegrationTest, FileSystemIntegration) {
    // 创建一个配置对象
    JsonObject config;
    config.set("app_name", "Test Application")
          .set("version", "1.0.0")
          .set("debug", true)
          .set("database", JsonObject{
              {"host", "localhost"},
              {"port", 5432},
              {"user", "admin"},
              {"password", "secret"}
          })
          .set("allowed_ips", JsonArray{"127.0.0.1", "192.168.1.1"});
    
    // 写入配置文件
    std::string config_file = "/tmp/json_config_test.json";
    std::ofstream out(config_file);
    out << config.dump(2);
    out.close();
    
    // 从文件读取配置
    std::ifstream in(config_file);
    std::stringstream buffer;
    buffer << in.rdbuf();
    in.close();
    
    Json loaded_config = Json::parse(buffer.str());
    ASSERT_FALSE(loaded_config.has_error());
    
    // 验证配置内容
    ASSERT_EQ(loaded_config["app_name"].get<std::string>(), "Test Application");
    ASSERT_EQ(loaded_config["version"].get<std::string>(), "1.0.0");
    ASSERT_EQ(loaded_config["debug"].get<bool>(), true);
    ASSERT_EQ(loaded_config["database"]["host"].get<std::string>(), "localhost");
    ASSERT_EQ(loaded_config["database"]["port"].get<int>(), 5432);
    ASSERT_EQ(loaded_config["allowed_ips"][0].get<std::string>(), "127.0.0.1");
    
    // 修改配置并保存
    loaded_config["version"] = "1.0.1";
    loaded_config["database"]["password"] = "new_secret";
    
    std::ofstream out_updated(config_file);
    out_updated << loaded_config.dump(2);
    out_updated.close();
    
    // 重新加载并验证
    std::ifstream in_updated(config_file);
    std::stringstream buffer_updated;
    buffer_updated << in_updated.rdbuf();
    in_updated.close();
    
    Json updated_config = Json::parse(buffer_updated.str());
    ASSERT_FALSE(updated_config.has_error());
    ASSERT_EQ(updated_config["version"].get<std::string>(), "1.0.1");
    ASSERT_EQ(updated_config["database"]["password"].get<std::string>(), "new_secret");
}

// 测试JSON与STL容器的集成
TEST(JsonIntegrationTest, StlContainerIntegration) {
    // 从STL容器创建JSON
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    std::map<std::string, std::string> dict = {
        {"key1", "value1"},
        {"key2", "value2"}
    };
    
    // 创建数组
    JsonArray json_array;
    for (int num : numbers) {
        json_array.push_back(num);
    }
    
    // 验证数组
    ASSERT_EQ(json_array.size(), numbers.size());
    for (size_t i = 0; i < numbers.size(); ++i) {
        ASSERT_EQ(json_array[i].get<int>(), numbers[i]);
    }
    
    // 创建对象
    JsonObject json_object;
    for (const auto& pair : dict) {
        json_object.set(pair.first, pair.second);
    }
    
    // 验证对象
    ASSERT_EQ(json_object.size(), dict.size());
    for (const auto& pair : dict) {
        ASSERT_EQ(json_object[pair.first].get<std::string>(), pair.second);
    }
    
    // 将JSON转换回STL容器
    std::vector<int> extracted_numbers;
    for (const auto& item : json_array) {
        extracted_numbers.push_back(item.get<int>());
    }
    
    std::map<std::string, std::string> extracted_dict;
    for (const auto& item : json_object) {
        extracted_dict[item.key()] = item.value().get<std::string>();
    }
    
    // 验证转换结果
    ASSERT_EQ(extracted_numbers, numbers);
    ASSERT_EQ(extracted_dict, dict);
}

// 测试JSON与自定义数据结构的集成
TEST(JsonIntegrationTest, CustomStructIntegration) {
    // 定义自定义数据结构
    struct User {
        int id;
        std::string name;
        std::string email;
        bool active;
        std::vector<std::string> roles;
        
        // 转换为JSON
        JsonObject to_json() const {
            JsonObject obj;
            obj.set("id", id)
               .set("name", name)
               .set("email", email)
               .set("active", active);
            
            JsonArray roles_array;
            for (const auto& role : roles) {
                roles_array.push_back(role);
            }
            obj.set("roles", roles_array);
            
            return obj;
        }
        
        // 从JSON创建
        static User from_json(const Json& json) {
            User user;
            user.id = json["id"].get<int>();
            user.name = json["name"].get<std::string>();
            user.email = json["email"].get<std::string>();
            user.active = json["active"].get<bool>();
            
            for (const auto& role : json["roles"]) {
                user.roles.push_back(role.get<std::string>());
            }
            
            return user;
        }
        
        // 相等运算符，用于测试
        bool operator==(const User& other) const {
            return id == other.id &&
                   name == other.name &&
                   email == other.email &&
                   active == other.active &&
                   roles == other.roles;
        }
    };
    
    // 创建用户对象
    User user{
        1001,
        "John Doe",
        "john.doe@example.com",
        true,
        {"user", "admin"}
    };
    
    // 转换为JSON
    JsonObject user_json = user.to_json();
    
    // 验证JSON
    ASSERT_EQ(user_json["id"].get<int>(), user.id);
    ASSERT_EQ(user_json["name"].get<std::string>(), user.name);
    ASSERT_EQ(user_json["email"].get<std::string>(), user.email);
    ASSERT_EQ(user_json["active"].get<bool>(), user.active);
    ASSERT_EQ(user_json["roles"][0].get<std::string>(), user.roles[0]);
    ASSERT_EQ(user_json["roles"][1].get<std::string>(), user.roles[1]);
    
    // 序列化和解析
    std::string json_str = user_json.dump();
    Json parsed = Json::parse(json_str);
    ASSERT_FALSE(parsed.has_error());
    
    // 从JSON创建用户
    User reconstructed_user = User::from_json(parsed);
    
    // 验证重建的用户
    ASSERT_EQ(reconstructed_user, user);
}

// 测试JSON与错误处理的集成
TEST(JsonIntegrationTest, ErrorHandlingIntegration) {
    // 模拟API响应处理
    auto process_response = [](const std::string& json_str) -> std::pair<bool, std::string> {
        Json response = Json::parse(json_str);
        
        if (response.has_error()) {
            return {false, "Invalid JSON: " + response.last_error()};
        }
        
        if (!response.is_object()) {
            return {false, "Expected JSON object"};
        }
        
        if (!response.has("status")) {
            return {false, "Missing 'status' field"};
        }
        
        std::string status;
        if (!response["status"].get(status)) {
            return {false, "Invalid 'status' field type"};
        }
        
        if (status == "error") {
            std::string message = response.has("message") ? 
                                 response["message"].get<std::string>() : 
                                 "Unknown error";
            return {false, message};
        }
        
        return {true, "Success"};
    };
    
    // 测试有效响应
    std::string valid_response = R"({"status":"success","data":{"id":1}})";
    auto valid_result = process_response(valid_response);
    ASSERT_TRUE(valid_result.first);
    ASSERT_EQ(valid_result.second, "Success");
    
    // 测试无效JSON
    std::string invalid_json = R"({"status":"success",})";
    auto invalid_json_result = process_response(invalid_json);
    ASSERT_FALSE(invalid_json_result.first);
    ASSERT_TRUE(invalid_json_result.second.find("Invalid JSON") != std::string::npos);
    
    // 测试缺少字段
    std::string missing_field = R"({"data":{"id":1}})";
    auto missing_field_result = process_response(missing_field);
    ASSERT_FALSE(missing_field_result.first);
    ASSERT_EQ(missing_field_result.second, "Missing 'status' field");
    
    // 测试类型错误
    std::string type_error = R"({"status":123})";
    auto type_error_result = process_response(type_error);
    ASSERT_FALSE(type_error_result.first);
    ASSERT_EQ(type_error_result.second, "Invalid 'status' field type");
    
    // 测试错误状态
    std::string error_status = R"({"status":"error","message":"Resource not found"})";
    auto error_status_result = process_response(error_status);
    ASSERT_FALSE(error_status_result.first);
    ASSERT_EQ(error_status_result.second, "Resource not found");
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 