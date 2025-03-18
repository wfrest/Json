#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <exception>

namespace test {

class TestFailure : public std::exception {
public:
    TestFailure(const std::string& message, const std::string& file, int line)
        : message_(message), file_(file), line_(line) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }

    std::string file() const { return file_; }
    int line() const { return line_; }

private:
    std::string message_;
    std::string file_;
    int line_;
};

class TestCase {
public:
    TestCase(const std::string& name, std::function<void()> testFunc)
        : name_(name), testFunc_(testFunc) {}

    bool Run() {
        std::cout << "[ RUN      ] " << name_ << std::endl;
        try {
            testFunc_();
            std::cout << "[       OK ] " << name_ << std::endl;
            return true;
        } catch (const TestFailure& e) {
            std::cout << "[  FAILED  ] " << name_ << std::endl;
            std::cout << e.file() << ":" << e.line() << ": " << e.what() << std::endl;
            return false;
        } catch (const std::exception& e) {
            std::cout << "[  FAILED  ] " << name_ << std::endl;
            std::cout << "Unexpected exception: " << e.what() << std::endl;
            return false;
        } catch (...) {
            std::cout << "[  FAILED  ] " << name_ << std::endl;
            std::cout << "Unknown exception" << std::endl;
            return false;
        }
    }

private:
    std::string name_;
    std::function<void()> testFunc_;
};

class TestSuite {
public:
    static TestSuite& GetInstance() {
        static TestSuite instance;
        return instance;
    }

    void AddTest(const std::string& name, std::function<void()> testFunc) {
        tests_.push_back(TestCase(name, testFunc));
    }

    int RunAll() {
        std::cout << "[==========] Running " << tests_.size() << " tests." << std::endl;
        
        size_t passed = 0;
        for (auto& test : tests_) {
            if (test.Run()) {
                passed++;
            }
        }
        
        std::cout << "[==========] " << tests_.size() << " tests ran." << std::endl;
        std::cout << "[  PASSED  ] " << passed << " tests." << std::endl;
        
        if (passed < tests_.size()) {
            std::cout << "[  FAILED  ] " << (tests_.size() - passed) << " tests." << std::endl;
            return 1;
        }
        
        return 0;
    }

private:
    std::vector<TestCase> tests_;
};

} // namespace test

#define TEST(test_suite, test_name) \
    void test_suite##_##test_name(); \
    struct test_suite##_##test_name##_register { \
        test_suite##_##test_name##_register() { \
            test::TestSuite::GetInstance().AddTest(#test_suite "." #test_name, test_suite##_##test_name); \
        } \
    } test_suite##_##test_name##_register_instance; \
    void test_suite##_##test_name()

#define EXPECT_TRUE(condition) \
    if (!(condition)) { \
        throw test::TestFailure("Expected true, got false: " #condition, __FILE__, __LINE__); \
    }

#define EXPECT_FALSE(condition) \
    if (condition) { \
        throw test::TestFailure("Expected false, got true: " #condition, __FILE__, __LINE__); \
    }

#define EXPECT_EQ(expected, actual) \
    if (!((expected) == (actual))) { \
        throw test::TestFailure(std::string("Expected equality of ") + #expected + " and " + #actual, __FILE__, __LINE__); \
    }

#define EXPECT_NE(expected, actual) \
    if ((expected) == (actual)) { \
        throw test::TestFailure(std::string("Expected inequality of ") + #expected + " and " + #actual, __FILE__, __LINE__); \
    }

#define ASSERT_TRUE(condition) EXPECT_TRUE(condition)
#define ASSERT_FALSE(condition) EXPECT_FALSE(condition)
#define ASSERT_EQ(expected, actual) EXPECT_EQ(expected, actual)
#define ASSERT_NE(expected, actual) EXPECT_NE(expected, actual)

#define MAIN_TEST() \
int main(int, char**) { \
    return test::TestSuite::GetInstance().RunAll(); \
}

#endif // TEST_UTILS_H 