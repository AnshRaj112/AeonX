#include "myc_test.hpp"

#include "myc/driver/cli_parser.hpp"

MYC_TEST(cli_parse_help_flag) {
    char arg0[] = "myc";
    char arg1[] = "--help";
    char* argv[] = {arg0, arg1};
    const auto options = myc::driver::CliParser::Parse(2, argv);
    MYC_ASSERT(options.show_help);
}

MYC_TEST(cli_parse_version_flag) {
    char arg0[] = "myc";
    char arg1[] = "--version";
    char* argv[] = {arg0, arg1};
    const auto options = myc::driver::CliParser::Parse(2, argv);
    MYC_ASSERT(options.show_version);
}

MYC_TEST(cli_parse_build_command) {
    char arg0[] = "myc";
    char arg1[] = "build";
    char arg2[] = "main.myc";
    char* argv[] = {arg0, arg1, arg2};
    const auto options = myc::driver::CliParser::Parse(3, argv);
    MYC_ASSERT_EQ(options.command, "build");
    MYC_ASSERT_EQ(options.source_files.size(), 1u);
}

MYC_TEST(cli_usage_is_non_empty) {
    MYC_ASSERT(!myc::driver::CliParser::GetUsage().empty());
}
