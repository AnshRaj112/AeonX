#include "myc_test.hpp"

namespace myc::test {

TestRegistry& TestRegistry::Instance() {
    static TestRegistry registry;
    return registry;
}

void TestRegistry::Register(std::string name, std::function<void()> body) {
    tests_.push_back(TestCase{std::move(name), std::move(body)});
}

int TestRegistry::RunAll() {
    int failures = 0;
    for (const auto& test : tests_) {
        try {
            test.body();
            std::cout << "[ PASS ] " << test.name << '\n';
        } catch (const std::exception& ex) {
            std::cerr << "[ FAIL ] " << test.name << ": " << ex.what() << '\n';
            ++failures;
        }
    }
    std::cout << failures << " failure(s) out of " << tests_.size() << " test(s)\n";
    return failures == 0 ? 0 : 1;
}

}  // namespace myc::test

int main() { return myc::test::TestRegistry::Instance().RunAll(); }
