#include "myc/driver/compiler_driver.hpp"

#include "myc/logging/logger.hpp"
#include "myc/version/version.hpp"

#include <iostream>
#include <string>

namespace myc::driver {

CompilerDriver::CompilerDriver()
    : dispatcher_(config_, diagnostics_, source_manager_) {}

int CompilerDriver::Run(int argc, char* argv[]) {
    logging::Logger::Instance().Info(
        std::string(version::kLanguageName) + " compiler starting");

    const CliOptions options = CliParser::Parse(argc, argv);
    LoadConfiguration(options);
    InitializeDiagnostics();

    const int exit_code = dispatcher_.Dispatch(options);

    if (diagnostics_.HasErrors()) {
        diagnostics_.PrintAll(std::cerr);
    }

    logging::Logger::Instance().Info("compiler exiting with code " +
                                     std::to_string(exit_code));
    return exit_code;
}

config::CompilerConfig& CompilerDriver::GetConfig() { return config_; }

const config::CompilerConfig& CompilerDriver::GetConfig() const { return config_; }

diagnostics::DiagnosticEngine& CompilerDriver::GetDiagnostics() { return diagnostics_; }

source::SourceManager& CompilerDriver::GetSourceManager() { return source_manager_; }

void CompilerDriver::LoadConfiguration(const CliOptions& options) {
    (void)options;
    config_.ApplyProfileDefaults(config::BuildProfile::Debug);
    logging::Logger::Instance().Debug("configuration loaded");
}

void CompilerDriver::InitializeDiagnostics() {
    diagnostics_.Clear();
    logging::Logger::Instance().Debug("diagnostics engine initialized");
}

}  // namespace myc::driver
