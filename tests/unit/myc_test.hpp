#pragma once

#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace myc::test {

struct TestCase {
    std::string name;
    std::function<void()> body;
};

class TestRegistry {
public:
    static TestRegistry& Instance();

    void Register(std::string name, std::function<void()> body);
    int RunAll();

private:
    std::vector<TestCase> tests_;
};

#define MYC_TEST(name)                                            \
    static void myc_test_##name();                                \
    static const bool myc_test_reg_##name = [] {                  \
        myc::test::TestRegistry::Instance().Register(#name, myc_test_##name); \
        return true;                                              \
    }();                                                          \
    static void myc_test_##name()

#define MYC_ASSERT(condition)                                     \
    do {                                                          \
        if (!(condition)) {                                       \
            throw std::runtime_error(                             \
                std::string("assertion failed: ") + #condition);  \
        }                                                         \
    } while (false)

#define MYC_ASSERT_EQ(a, b)                                       \
    do {                                                          \
        const auto va = (a);                                      \
        const auto vb = (b);                                      \
        if (va != vb) {                                           \
            throw std::runtime_error("assertion failed: " #a " == " #b); \
        }                                                         \
    } while (false)

}  // namespace myc::test
