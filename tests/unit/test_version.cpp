#include "myc_test.hpp"

#include "myc/version/version.hpp"

#include <string>

MYC_TEST(version_language_name) {
    MYC_ASSERT_EQ(myc::version::kLanguageName, "Myc");
}

MYC_TEST(version_compiler_version) {
    MYC_ASSERT_EQ(myc::version::kCompilerVersion, "0.1.0");
}

MYC_TEST(version_full_string_contains_compiler) {
    const std::string full = myc::version::GetFullVersionString();
    MYC_ASSERT(full.find("Myc compiler") != std::string::npos);
}
