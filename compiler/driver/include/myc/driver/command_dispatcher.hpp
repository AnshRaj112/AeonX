#pragma once

#include "myc/config/compiler_config.hpp"
#include "myc/diagnostics/diagnostic_engine.hpp"
#include "myc/driver/cli_parser.hpp"
#include "myc/source/source_manager.hpp"

namespace myc::driver {

/// Dispatches parsed CLI commands to their handlers.
class CommandDispatcher {
public:
    CommandDispatcher(config::CompilerConfig& config,
                      diagnostics::DiagnosticEngine& diagnostics,
                      source::SourceManager& source_manager);

    /// Executes the requested command. Returns process exit code.
    [[nodiscard]] int Dispatch(const CliOptions& options);

private:
    [[nodiscard]] int RunBuild(const CliOptions& options);
    [[nodiscard]] int RunRun(const CliOptions& options);
    [[nodiscard]] int RunFmt(const CliOptions& options);
    [[nodiscard]] int RunLint(const CliOptions& options);
    [[nodiscard]] int RunBenchmark(const CliOptions& options);

    config::CompilerConfig& config_;
    diagnostics::DiagnosticEngine& diagnostics_;
    source::SourceManager& source_manager_;
};

}  // namespace myc::driver
