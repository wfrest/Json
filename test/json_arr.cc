#include "Json.h"
#include "test/test_utils.h"

using namespace wfrest;

TEST(ArrTest, create_arr)
{
    Json data = Json::Array{1, true, "string", nullptr, "123"};
    ASSERT_EQ(data.dump(), R"([1,true,"string",null,"123"])");
}

TEST(ArrTest, empty_arr)
{
    Json data = Json::Array();
    ASSERT_TRUE(data.is_array());
    ASSERT_EQ(data.dump(), "[]");
}

TEST(ArrTest, arr_push)
{
    Json data;
    data.push_back(1);
    data.push_back(nullptr);
    data.push_back("string");
    data.push_back(true);
    data.push_back(false);
    ASSERT_EQ(data.dump(), R"([1,null,"string",true,false])");
}

TEST(ArrTest, arr_update)
{
    Json data;
    data.push_back(1);
    data.push_back(nullptr);
    data.push_back("string");
    data.push_back(true);
    data.push_back(false);
    ASSERT_EQ(data.dump(), R"([1,null,"string",true,false])");
    data[2] = 1;
    ASSERT_EQ(data[2].get<int>(), 1);
    data[2] = 2.0;
    ASSERT_EQ(data[2].get<double>(), 2.0);
    data[2] = "123";
    ASSERT_EQ(data[2].get<std::string>(), "123");
    data[2] = nullptr;
    ASSERT_EQ(data[2].get<std::nullptr_t>(), nullptr);
}

TEST(ArrTest, arr_search)
{
    Json data;
    data.push_back(1);        // 0
    data.push_back(2.1);      // 1
    data.push_back(nullptr);  // 2
    data.push_back("string"); // 3
    data.push_back(true);     // 4
    data.push_back(false);    // 5

    ASSERT_EQ(data[0].get<int>(), 1);
    ASSERT_EQ(data[1].get<double>(), 2.1);
    ASSERT_EQ(data[2].get<std::nullptr_t>(), nullptr);
    ASSERT_EQ(data[3].get<std::string>(), "string");
    ASSERT_EQ(data[4].get<bool>(), true);
    ASSERT_EQ(data[5].get<bool>(), false);

    // Object
    Json::Object obj;
    obj["123"] = 12;
    obj["123"]["1"] = "test";
    // todo : we need a move interface
    // we copy here
    data.push_back(obj); // 6

    // std::cout << data[6] << std::endl;
    // std::cout << data[6].get<Json::Object>().dump() << std::endl;
    ASSERT_EQ(data[6].get<Json::Object>().dump(), R"({"123":12})");

    // Array
    Json::Array arr;
    arr.push_back(1);
    arr.push_back(nullptr);

    data.push_back(arr);

    // std::cout << data[7] << std::endl;
    // std::cout << data[7].get<Json::Array>().dump() << std::endl;
    ASSERT_EQ(data[7].get<Json::Array>().dump(), R"([1,null])");

    // implicit conversion
    int a = data[0];
    ASSERT_EQ(a, 1);
    double b = data[1];
    ASSERT_EQ(b, 2.1);
    std::nullptr_t c = data[2];
    ASSERT_EQ(c, nullptr);
    std::string d = data[3];
    ASSERT_EQ(d, "string");
    bool e = data[4];
    ASSERT_EQ(e, true);
    bool f = data[5];
    ASSERT_EQ(f, false);

    Json::Object g = data[6];
    ASSERT_EQ(g.dump(), R"({"123":12})");

    Json::Array h = data[7];
    ASSERT_EQ(h.dump(), R"([1,null])");
}

TEST(ArrTest, erase)
{
    Json data;
    data.push_back(1);
    data.push_back(nullptr);
    data.push_back("string");
    data.push_back(true);
    data.push_back(false);
    ASSERT_EQ(data.dump(), R"([1,null,"string",true,false])");
    data.erase(2);
    ASSERT_EQ(data.dump(), R"([1,null,true,false])");
}

TEST(ArrTest, push_vector) {
    Json data;
    std::vector<std::string> values = {"val1", "val2"};
    data.push_back(values);

    ASSERT_EQ(data[0].get<std::string>(), "val1");
    ASSERT_EQ(data[1].get<std::string>(), "val2");

    data.push_back({"val3", "val4", "val5"});
    ASSERT_EQ(data[2].get<std::string>(), "val3");
    ASSERT_EQ(data[3].get<std::string>(), "val4");
    ASSERT_EQ(data[4].get<std::string>(), "val5");
}

// Add the main function for our test framework
MAIN_TEST()
