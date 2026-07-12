#include "myc_test.hpp"

#include "myc/cli/cli_parser.hpp"
#include "myc/commands/command_registry.hpp"
#include "myc/driver/command_dispatcher.hpp"
#include "myc/exitcodes/exit_codes.hpp"

MYC_TEST(cli_parse_help_flag) {
    char arg0[] = "myc";
    char arg1[] = "--help";
    char* argv[] = {arg0, arg1};
    const auto result = myc::cli::CliParser::Parse(2, argv);
    MYC_ASSERT(result.has_value());
    MYC_ASSERT(result->show_help);
}

MYC_TEST(cli_parse_version_flag) {
    char arg0[] = "myc";
    char arg1[] = "--version";
    char* argv[] = {arg0, arg1};
    const auto result = myc::cli::CliParser::Parse(2, argv);
    MYC_ASSERT(result.has_value());
    MYC_ASSERT(result->show_version);
}

MYC_TEST(cli_parse_build_command) {
    char arg0[] = "myc";
    char arg1[] = "build";
    char arg2[] = "main.myc";
    char* argv[] = {arg0, arg1, arg2};
    const auto result = myc::cli::CliParser::Parse(3, argv);
    MYC_ASSERT(result.has_value());
    MYC_ASSERT_EQ(result->command, "build");
    MYC_ASSERT_EQ(result->source_files.size(), 1u);
}

MYC_TEST(cli_parse_global_flags) {
    char arg0[] = "myc";
    char arg1[] = "--verbose";
    char arg2[] = "--profile=release";
    char arg3[] = "--target";
    char arg4[] = "aarch64";
    char arg5[] = "build";
    char* argv[] = {arg0, arg1, arg2, arg3, arg4, arg5};
    const auto result = myc::cli::CliParser::Parse(6, argv);
    MYC_ASSERT(result.has_value());
    MYC_ASSERT(result->verbose);
    MYC_ASSERT_EQ(*result->profile, "release");
    MYC_ASSERT_EQ(*result->target, "aarch64");
    MYC_ASSERT_EQ(result->command, "build");
}

MYC_TEST(cli_parse_invalid_verbose_and_quiet) {
    char arg0[] = "myc";
    char arg1[] = "--verbose";
    char arg2[] = "--quiet";
    char* argv[] = {arg0, arg1, arg2};
    const auto result = myc::cli::CliParser::Parse(3, argv);
    MYC_ASSERT(!result.has_value());
}

MYC_TEST(cli_parse_missing_profile_value) {
    char arg0[] = "myc";
    char arg1[] = "--profile";
    char* argv[] = {arg0, arg1};
    const auto result = myc::cli::CliParser::Parse(2, argv);
    MYC_ASSERT(!result.has_value());
}

MYC_TEST(cli_usage_is_non_empty) {
    MYC_ASSERT(!myc::cli::CliParser::GetUsage().empty());
}

MYC_TEST(command_registry_has_builtin_commands) {
    myc::commands::EnsureCommandsRegistered();
    const auto& registry = myc::commands::CommandRegistry::Instance();
    MYC_ASSERT(registry.IsRegistered("build"));
    MYC_ASSERT(registry.IsRegistered("run"));
    MYC_ASSERT(registry.IsRegistered("fmt"));
    MYC_ASSERT(registry.IsRegistered("lint"));
    MYC_ASSERT(registry.IsRegistered("test"));
    MYC_ASSERT(registry.IsRegistered("repl"));
}

MYC_TEST(command_registry_unknown_command) {
    myc::commands::EnsureCommandsRegistered();
    MYC_ASSERT(!myc::commands::CommandRegistry::Instance().Find("not-a-command"));
}

MYC_TEST(exit_codes_are_distinct) {
    MYC_ASSERT(myc::exitcodes::ToInt(myc::exitcodes::ExitCode::Success) == 0);
    MYC_ASSERT(myc::exitcodes::ToInt(myc::exitcodes::ExitCode::InvalidArguments) == 2);
    MYC_ASSERT(myc::exitcodes::ToInt(myc::exitcodes::ExitCode::InternalCompilerError) == 5);
}

MYC_TEST(dispatcher_unknown_command) {
    myc::config::CompilerConfig config;
    myc::diagnostics::DiagnosticEngine diagnostics;
    myc::source::SourceManager source_manager;
    myc::driver::CommandDispatcher dispatcher(config, diagnostics, source_manager);

    myc::cli::CliOptions options;
    options.command = "unknown-command";
    const int code = dispatcher.Dispatch(options);
    MYC_ASSERT_EQ(code, 2);
    MYC_ASSERT(diagnostics.HasErrors());
}

MYC_TEST(dispatcher_help_command) {
    myc::config::CompilerConfig config;
    myc::diagnostics::DiagnosticEngine diagnostics;
    myc::source::SourceManager source_manager;
    myc::driver::CommandDispatcher dispatcher(config, diagnostics, source_manager);

    myc::cli::CliOptions options;
    options.show_help = true;
    const int code = dispatcher.Dispatch(options);
    MYC_ASSERT_EQ(code, 0);
}

MYC_TEST(dispatcher_version_command) {
    myc::config::CompilerConfig config;
    myc::diagnostics::DiagnosticEngine diagnostics;
    myc::source::SourceManager source_manager;
    myc::driver::CommandDispatcher dispatcher(config, diagnostics, source_manager);

    myc::cli::CliOptions options;
    options.show_version = true;
    const int code = dispatcher.Dispatch(options);
    MYC_ASSERT_EQ(code, 0);
}

MYC_TEST(dispatcher_no_command) {
    myc::config::CompilerConfig config;
    myc::diagnostics::DiagnosticEngine diagnostics;
    myc::source::SourceManager source_manager;
    myc::driver::CommandDispatcher dispatcher(config, diagnostics, source_manager);

    myc::cli::CliOptions options;
    const int code = dispatcher.Dispatch(options);
    MYC_ASSERT_EQ(code, 2);
}
