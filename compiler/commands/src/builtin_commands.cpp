#include "myc/commands/builtin_commands.hpp"

#include "myc/common/cli_output.hpp"
#include "myc/logging/logger.hpp"

#include <filesystem>
#include <iostream>

namespace myc::commands {

namespace {

exitcodes::ExitCode RunWithBanner(const CommandContext& ctx, std::string_view command,
                                  std::string_view message) {
    logging::Logger::Instance().Info(std::string(command) + ": " + std::string(message));

    if (!ctx.options.quiet) {
        common::PrintBanner(std::cout, ctx.config);
        common::PrintPlaceholder(std::cout, command, message);
    }

    return exitcodes::ExitCode::Success;
}

void LogSourceFiles(std::string_view label, const CommandContext& ctx) {
    for (const auto& file : ctx.options.source_files) {
        logging::Logger::Instance().Info(std::string(label) + ": " + file);
    }
}

}  // namespace

std::string_view BuildCommand::GetName() const noexcept { return "build"; }

std::string_view BuildCommand::GetDescription() const noexcept {
    return "Compile .myc source files into object files and binaries";
}

std::string_view BuildCommand::GetSynopsis() const noexcept {
    return "myc build [options] <files...>";
}

exitcodes::ExitCode BuildCommand::Execute(const CommandContext& ctx) const {
    const auto result = RunWithBanner(
        ctx, GetName(),
        "Compilation pipeline (lexer, parser, semantic analysis, codegen) is not yet "
        "implemented.");
    LogSourceFiles("build input", ctx);

    for (const auto& file : ctx.options.source_files) {
        if (!std::filesystem::exists(file)) {
            ctx.diagnostics.EmitError("D0001", "source file not found: " + file);
            return exitcodes::ExitCode::FileNotFound;
        }
        (void)ctx.source_manager.LoadFile(file);
    }

    return result;
}

std::string_view RunCommand::GetName() const noexcept { return "run"; }

std::string_view RunCommand::GetDescription() const noexcept {
    return "Build and execute a Myc program";
}

std::string_view RunCommand::GetSynopsis() const noexcept {
    return "myc run [options] <entry.myc>";
}

exitcodes::ExitCode RunCommand::Execute(const CommandContext& ctx) const {
  const auto result = RunWithBanner(
      ctx, GetName(),
      "Program execution requires a completed build pipeline and runtime, not yet available.");
    if (!ctx.options.source_files.empty()) {
        logging::Logger::Instance().Info("run entry: " + ctx.options.source_files.front());
    }
    return result;
}

std::string_view FormatCommand::GetName() const noexcept { return "fmt"; }

std::string_view FormatCommand::GetDescription() const noexcept {
    return "Format Myc source code according to language style guidelines";
}

std::string_view FormatCommand::GetSynopsis() const noexcept {
    return "myc fmt [options] <files...>";
}

exitcodes::ExitCode FormatCommand::Execute(const CommandContext& ctx) const {
    const auto result =
        RunWithBanner(ctx, GetName(), "Source formatter integration is planned for a future release.");
    LogSourceFiles("fmt target", ctx);
    return result;
}

std::string_view LintCommand::GetName() const noexcept { return "lint"; }

std::string_view LintCommand::GetDescription() const noexcept {
    return "Run static analysis and lint checks on Myc source files";
}

std::string_view LintCommand::GetSynopsis() const noexcept {
    return "myc lint [options] <files...>";
}

exitcodes::ExitCode LintCommand::Execute(const CommandContext& ctx) const {
    const auto result =
        RunWithBanner(ctx, GetName(), "Linter integration is planned for a future release.");
    LogSourceFiles("lint target", ctx);
    return result;
}

std::string_view BenchmarkCommand::GetName() const noexcept { return "benchmark"; }

std::string_view BenchmarkCommand::GetDescription() const noexcept {
    return "Run compiler and runtime performance benchmarks";
}

std::string_view BenchmarkCommand::GetSynopsis() const noexcept {
    return "myc benchmark [options]";
}

exitcodes::ExitCode BenchmarkCommand::Execute(const CommandContext& ctx) const {
    return RunWithBanner(ctx, GetName(),
                         "Benchmark harness will measure compile-time and runtime performance.");
}

}  // namespace myc::commands
