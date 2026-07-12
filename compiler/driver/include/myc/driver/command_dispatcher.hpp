#pragma once

#include "myc/cli/cli_options.hpp"
#include "myc/commands/abstract_command.hpp"
#include "myc/config/compiler_config.hpp"
#include "myc/diagnostics/diagnostic_engine.hpp"
#include "myc/source/source_manager.hpp"

namespace myc::driver {

/// Dispatches parsed CLI commands through the command registry.
class CommandDispatcher {
public:
    CommandDispatcher(config::CompilerConfig& config,
                      diagnostics::DiagnosticEngine& diagnostics,
                      source::SourceManager& source_manager);

    /// Executes the requested command. Returns process exit code.
    [[nodiscard]] int Dispatch(const cli::CliOptions& options);

private:
    [[nodiscard]] int ExecuteCommand(const commands::AbstractCommand& command,
                                     const cli::CliOptions& options);

    config::CompilerConfig& config_;
    diagnostics::DiagnosticEngine& diagnostics_;
    source::SourceManager& source_manager_;
};

}  // namespace myc::driver
