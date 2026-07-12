#include "myc/driver/compiler_driver.hpp"

#include "myc/cli/cli_parser.hpp"
#include "myc/diagnostics/diagnostic_reporter.hpp"
#include "myc/exitcodes/exit_codes.hpp"
#include "myc/logging/logger.hpp"
#include "myc/version/version.hpp"

#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace myc::driver {

namespace {

std::optional<config::BuildProfile> ParseProfile(std::string_view value) {
    if (value == "debug") {
        return config::BuildProfile::Debug;
    }
    if (value == "release") {
        return config::BuildProfile::Release;
    }
    if (value == "relwithdebinfo") {
        return config::BuildProfile::RelWithDebInfo;
    }
    if (value == "minsizerel") {
        return config::BuildProfile::MinSizeRel;
    }
    if (value == "custom") {
        return config::BuildProfile::Custom;
    }
    return std::nullopt;
}

std::optional<config::TargetArchitecture> ParseTarget(std::string_view value) {
    if (value == "x86_64" || value == "amd64") {
        return config::TargetArchitecture::X86_64;
    }
    if (value == "aarch64" || value == "arm64") {
        return config::TargetArchitecture::AArch64;
    }
    if (value == "wasm32") {
        return config::TargetArchitecture::Wasm32;
    }
    if (value == "wasm64") {
        return config::TargetArchitecture::Wasm64;
    }
    if (value == "riscv64") {
        return config::TargetArchitecture::RiscV64;
    }
    return std::nullopt;
}

std::optional<config::TargetPlatform> ParsePlatform(std::string_view value) {
    if (value == "linux") {
        return config::TargetPlatform::Linux;
    }
    if (value == "windows") {
        return config::TargetPlatform::Windows;
    }
    if (value == "macos" || value == "darwin") {
        return config::TargetPlatform::MacOS;
    }
    if (value == "wasm") {
        return config::TargetPlatform::Wasm;
    }
    if (value == "baremetal") {
        return config::TargetPlatform::BareMetal;
    }
    return std::nullopt;
}

std::optional<config::OptimizationLevel> ParseOptimization(std::string_view value) {
    if (value == "O0" || value == "0") {
        return config::OptimizationLevel::O0;
    }
    if (value == "O1" || value == "1") {
        return config::OptimizationLevel::O1;
    }
    if (value == "O2" || value == "2") {
        return config::OptimizationLevel::O2;
    }
    if (value == "O3" || value == "3") {
        return config::OptimizationLevel::O3;
    }
    if (value == "Os" || value == "s") {
        return config::OptimizationLevel::Os;
    }
    if (value == "Oz" || value == "z") {
        return config::OptimizationLevel::Oz;
    }
    return std::nullopt;
}

const char* GetEnv(std::string_view name) {
    return std::getenv(std::string(name).c_str());
}

bool IsColorEnabled(const cli::CliOptions& options) {
    switch (options.color_mode) {
        case cli::ColorMode::Always:
            return true;
        case cli::ColorMode::Never:
            return false;
        case cli::ColorMode::Auto:
#ifdef _WIN32
            return _isatty(_fileno(stderr)) != 0;
#else
            return isatty(fileno(stderr)) != 0;
#endif
    }
    return false;
}

diagnostics::DiagnosticReporter::Options BuildReporterOptions(
    const cli::CliOptions& options, const source::SourceManager& source_manager) {
    diagnostics::DiagnosticReporter::Options reporter_options;
    if (options.json_output || options.diagnostics_output) {
        reporter_options.format = diagnostics::DiagnosticOutputFormat::Json;
    } else {
        reporter_options.format = diagnostics::DiagnosticOutputFormat::PlainText;
    }
    reporter_options.colors_enabled = IsColorEnabled(options);
    reporter_options.source_manager = &source_manager;
    return reporter_options;
}

}  // namespace

CompilerDriver::CompilerDriver()
    : dispatcher_(config_, diagnostics_, source_manager_) {}

int CompilerDriver::Run(int argc, char* argv[]) {
    try {
        logging::Logger::Instance().Info(
            std::string(version::kLanguageName) + " compiler starting");

        const auto parse_result = cli::CliParser::Parse(argc, argv);
        if (!parse_result) {
            return HandleParseError(parse_result.error());
        }

        const cli::CliOptions& options = *parse_result;
        ConfigureLogging(options);
        LoadConfiguration(options);
        LoadEnvironmentOverrides();
        InitializeDiagnostics(options);

        const int exit_code = dispatcher_.Dispatch(options);

        if (diagnostics_.HasErrors()) {
            const diagnostics::DiagnosticReporter reporter(
                BuildReporterOptions(options, source_manager_));
            reporter.Report(diagnostics_, std::cerr);
        }

        logging::Logger::Instance().Info("compiler exiting with code " +
                                         std::to_string(exit_code));
        return exit_code;
    } catch (const std::exception& ex) {
        diagnostics_.Clear();
        diagnostics_.EmitError("D9999", std::string("internal compiler error: ") + ex.what());
        const diagnostics::DiagnosticReporter reporter;
        reporter.Report(diagnostics_, std::cerr);
        logging::Logger::Instance().Error(std::string("unhandled exception: ") + ex.what());
        return exitcodes::ToInt(exitcodes::ExitCode::InternalCompilerError);
    } catch (...) {
        diagnostics_.Clear();
        diagnostics_.EmitError("D9999", "internal compiler error: unknown exception");
        const diagnostics::DiagnosticReporter reporter;
        reporter.Report(diagnostics_, std::cerr);
        logging::Logger::Instance().Error("unhandled unknown exception");
        return exitcodes::ToInt(exitcodes::ExitCode::InternalCompilerError);
    }
}

config::CompilerConfig& CompilerDriver::GetConfig() { return config_; }

const config::CompilerConfig& CompilerDriver::GetConfig() const { return config_; }

diagnostics::DiagnosticEngine& CompilerDriver::GetDiagnostics() { return diagnostics_; }

source::SourceManager& CompilerDriver::GetSourceManager() { return source_manager_; }

void CompilerDriver::ConfigureLogging(const cli::CliOptions& options) {
    auto& logger = logging::Logger::Instance();
    if (options.quiet) {
        logger.SetLevel(logging::LogLevel::Warning);
    } else if (options.trace) {
        logger.SetLevel(logging::LogLevel::Trace);
    } else if (options.verbose) {
        logger.SetLevel(logging::LogLevel::Debug);
    } else {
        logger.SetLevel(logging::LogLevel::Info);
    }
    logging::Logger::Instance().Debug("logging configured");
}

void CompilerDriver::LoadConfiguration(const cli::CliOptions& options) {
    if (options.profile) {
        if (const auto profile = ParseProfile(*options.profile)) {
            config_.ApplyProfileDefaults(*profile);
        } else {
            diagnostics_.EmitError("D0004", "invalid build profile: " + *options.profile);
        }
    } else {
        config_.ApplyProfileDefaults(config::BuildProfile::Debug);
    }

    if (options.target) {
        if (const auto target = ParseTarget(*options.target)) {
            config_.SetTargetArchitecture(*target);
        } else {
            diagnostics_.EmitError("D0005", "invalid target architecture: " + *options.target);
        }
    } else {
        config_.SetTargetArchitecture(config::TargetArchitecture::X86_64);
    }

    if (options.platform) {
        if (const auto platform = ParsePlatform(*options.platform)) {
            config_.SetTargetPlatform(*platform);
        } else {
            diagnostics_.EmitError("D0006", "invalid target platform: " + *options.platform);
        }
    }

    if (options.optimization) {
        if (const auto opt = ParseOptimization(*options.optimization)) {
            config_.SetOptimizationLevel(*opt);
        } else {
            diagnostics_.EmitError("D0007",
                                   "invalid optimization level: " + *options.optimization);
        }
    }

    config::DiagnosticConfiguration diag_config = config_.GetDiagnosticConfiguration();
    if (options.diagnostics_output) {
        diag_config.show_notes = true;
        diag_config.show_hints = true;
    }
    config_.SetDiagnosticConfiguration(diag_config);

    logging::Logger::Instance().Debug("configuration loaded from CLI options");
}

void CompilerDriver::LoadEnvironmentOverrides() {
    if (const char* profile = GetEnv("MYC_PROFILE")) {
        if (const auto parsed = ParseProfile(profile)) {
            config_.ApplyProfileDefaults(*parsed);
            logging::Logger::Instance().Debug("MYC_PROFILE override applied");
        }
    }

    if (const char* target = GetEnv("MYC_TARGET")) {
        if (const auto parsed = ParseTarget(target)) {
            config_.SetTargetArchitecture(*parsed);
            logging::Logger::Instance().Debug("MYC_TARGET override applied");
        }
    }

    if (const char* platform = GetEnv("MYC_PLATFORM")) {
        if (const auto parsed = ParsePlatform(platform)) {
            config_.SetTargetPlatform(*parsed);
            logging::Logger::Instance().Debug("MYC_PLATFORM override applied");
        }
    }

    if (const char* verbose = GetEnv("MYC_VERBOSE")) {
        if (std::string(verbose) == "1" || std::string(verbose) == "true") {
            logging::Logger::Instance().SetLevel(logging::LogLevel::Debug);
        }
    }
}

void CompilerDriver::InitializeDiagnostics(const cli::CliOptions& options) {
    diagnostics_.Clear();
    source_manager_.SetDiagnosticsEngine(&diagnostics_);

    const config::DiagnosticConfiguration& cfg = config_.GetDiagnosticConfiguration();
    diagnostics::DiagnosticsPolicy policy{
        .warnings_as_errors = cfg.warnings_as_errors,
        .suppress_warnings = cfg.suppress_warnings,
        .show_notes = cfg.show_notes,
        .show_hints = cfg.show_hints,
        .error_recovery = cfg.error_recovery,
        .max_diagnostics = cfg.max_diagnostics,
        .error_limit = cfg.error_limit,
        .warning_limit = cfg.warning_limit,
    };
    diagnostics_.SetPolicy(policy);

    (void)options;
    logging::Logger::Instance().Debug("diagnostics engine initialized");
}

int CompilerDriver::HandleParseError(const std::string& message) {
    diagnostics_.EmitError("D0001", message);
    const diagnostics::DiagnosticReporter reporter;
    reporter.Report(diagnostics_, std::cerr);
    logging::Logger::Instance().Error("CLI parse error: " + message);
    return exitcodes::ToInt(exitcodes::ExitCode::InvalidArguments);
}

}  // namespace myc::driver
