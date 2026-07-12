#pragma once

#include "myc/cli/cli_options.hpp"
#include "myc/config/compiler_config.hpp"
#include "myc/diagnostics/diagnostic_engine.hpp"
#include "myc/source/source_manager.hpp"

namespace myc::commands {

/// Execution context shared by all commands.
struct CommandContext {
    config::CompilerConfig& config;
    diagnostics::DiagnosticEngine& diagnostics;
    source::SourceManager& source_manager;
    const cli::CliOptions& options;
};

}  // namespace myc::commands
