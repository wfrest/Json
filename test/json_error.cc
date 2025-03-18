#include "wfrest/Json.h"
#include "test/test_utils.h"
#include <fstream>
#include <sstream>
#include <string>

using namespace wfrest;

class JsonErrorTest : public testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

// Test error handling for parsing invalid JSON
TEST_F(JsonErrorTest, ParseErrors) {
    // Invalid JSON syntax
    Json json = Json::parse("{invalid:json}");
    ASSERT_TRUE(json.has_error());
    ASSERT_EQ(Json::PARSE_ERROR, json.error_code());
    ASSERT_FALSE(json.last_error().empty());

    // Unbalanced brackets
    json = Json::parse("{\"key\": \"value\"");
    ASSERT_TRUE(json.has_error());
    ASSERT_EQ(Json::PARSE_ERROR, json.error_code());

    // Invalid escape sequence
    json = Json::parse("{\"key\": \"value\\z\"}");
    ASSERT_TRUE(json.has_error());
    ASSERT_EQ(Json::PARSE_ERROR, json.error_code());
}

// Test error handling for type mismatches
TEST_F(JsonErrorTest, TypeErrors) {
    // Create a JSON object
    Json json = Json::parse("{\"number\": 42, \"string\": \"text\", \"bool\": true, \"null\": null, \"array\": [1, 2, 3], \"object\": {\"key\": \"value\"}}");
    ASSERT_FALSE(json.has_error());

    // Try to get a number as a string
    std::string str_val;
    bool success = json["number"].get(str_val);
    ASSERT_FALSE(success);
    ASSERT_TRUE(json["number"].has_error());
    ASSERT_EQ(Json::TYPE_ERROR, json["number"].error_code());

    // Try to get a string as a number
    int int_val;
    success = json["string"].get(int_val);
    ASSERT_FALSE(success);
    ASSERT_TRUE(json["string"].has_error());
    ASSERT_EQ(Json::TYPE_ERROR, json["string"].error_code());

    // Try to get a boolean as a number
    success = json["bool"].get(int_val);
    ASSERT_FALSE(success);
    ASSERT_TRUE(json["bool"].has_error());
    ASSERT_EQ(Json::TYPE_ERROR, json["bool"].error_code());

    // Try to get null as a string
    success = json["null"].get(str_val);
    ASSERT_FALSE(success);
    ASSERT_TRUE(json["null"].has_error());
    ASSERT_EQ(Json::TYPE_ERROR, json["null"].error_code());
}

// Test error handling for accessing non-existent keys
TEST_F(JsonErrorTest, KeyErrors) {
    Json json = Json::parse("{\"key\": \"value\"}");
    ASSERT_FALSE(json.has_error());

    // Access non-existent key
    Json& non_existent = json["non_existent"];
    ASSERT_TRUE(non_existent.has_error());
    ASSERT_EQ(Json::KEY_ERROR, non_existent.error_code());
}

// Test error handling for accessing invalid array indices
TEST_F(JsonErrorTest, IndexErrors) {
    Json json = Json::parse("[1, 2, 3]");
    ASSERT_FALSE(json.has_error());

    // Access out-of-bounds index
    Json& out_of_bounds = json[10];
    ASSERT_TRUE(out_of_bounds.has_error());
    ASSERT_EQ(Json::INDEX_ERROR, out_of_bounds.error_code());

    // Access negative index
    Json& negative_index = json[-1];
    ASSERT_TRUE(negative_index.has_error());
    ASSERT_EQ(Json::INDEX_ERROR, negative_index.error_code());
}

// Test error recovery
TEST_F(JsonErrorTest, ErrorRecovery) {
    Json json = Json::parse("{\"key\": \"value\"}");
    ASSERT_FALSE(json.has_error());

    // Cause an error
    int int_val;
    bool success = json["key"].get(int_val);
    ASSERT_FALSE(success);
    ASSERT_TRUE(json["key"].has_error());

    // Clear the error
    json["key"].clear_error();
    ASSERT_FALSE(json["key"].has_error());

    // Now we should be able to use the JSON object again
    std::string str_val;
    success = json["key"].get(str_val);
    ASSERT_TRUE(success);
    ASSERT_EQ("value", str_val);
}

// Test error handling for file operations
TEST_F(JsonErrorTest, FileErrors) {
    // Try to parse from a non-existent file
    FILE* fp = fopen("non_existent_file.json", "r");
    ASSERT_EQ(nullptr, fp);

    // This should result in an error
    Json json = Json::parse(fp);
    ASSERT_TRUE(json.has_error());
    ASSERT_EQ(Json::IO_ERROR, json.error_code());

    // Clean up if file was somehow opened
    if (fp) {
        fclose(fp);
    }
}

// Test error handling for stream operations
TEST_F(JsonErrorTest, StreamErrors) {
    // Empty stream
    std::istringstream empty_stream("");
    Json json = Json::parse(empty_stream);
    ASSERT_TRUE(json.has_error());
    ASSERT_EQ(Json::PARSE_ERROR, json.error_code());

    // Invalid JSON in stream
    std::istringstream invalid_stream("not valid json");
    json = Json::parse(invalid_stream);
    ASSERT_TRUE(json.has_error());
    ASSERT_EQ(Json::PARSE_ERROR, json.error_code());
}

// Test error handling for large JSON
TEST_F(JsonErrorTest, LargeJsonErrors) {
    // Create a very large JSON string that might cause memory issues
    std::string large_json = "{";
    for (int i = 0; i < 10000; i++) {
        large_json += "\"key" + std::to_string(i) + "\":\"value" + std::to_string(i) + "\",";
    }
    large_json += "\"last_key\":\"last_value\"}";

    // Parse the large JSON
    Json json = Json::parse(large_json);
    
    // This might succeed or fail depending on the implementation's limits
    if (json.has_error() && json.error_code() == Json::MEMORY_ERROR) {
        ASSERT_EQ(Json::MEMORY_ERROR, json.error_code());
    } else {
        // If it succeeded, check that we can access the data
        ASSERT_EQ("last_value", json["last_key"].get<std::string>());
    }
}

// Test error handling for deeply nested JSON
TEST_F(JsonErrorTest, DeepNestingErrors) {
    // Create a deeply nested JSON structure
    std::string deep_json = "{";
    for (int i = 0; i < 100; i++) {
        deep_json += "\"level" + std::to_string(i) + "\":{";
    }
    deep_json += "\"value\":42";
    for (int i = 0; i < 100; i++) {
        deep_json += "}";
    }
    deep_json += "}";

    // Parse the deeply nested JSON
    Json json = Json::parse(deep_json);
    
    // This might succeed or fail depending on the implementation's limits
    if (json.has_error() && json.error_code() == Json::PARSE_ERROR) {
        ASSERT_EQ(Json::PARSE_ERROR, json.error_code());
    } else {
        // If it succeeded, try to access the deeply nested value
        Json current = json;
        for (int i = 0; i < 100; i++) {
            current = current["level" + std::to_string(i)];
        }
        ASSERT_EQ(42, current["value"].get<int>());
    }
}

// Test error handling for empty JSON
TEST_F(JsonErrorTest, EmptyJsonErrors) {
    // Empty string
    Json json = Json::parse("");
    ASSERT_TRUE(json.has_error());
    ASSERT_EQ(Json::PARSE_ERROR, json.error_code());

    // Just whitespace
    json = Json::parse("   \n\t   ");
    ASSERT_TRUE(json.has_error());
    ASSERT_EQ(Json::PARSE_ERROR, json.error_code());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 