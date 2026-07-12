#pragma once

#include "myc/config/compiler_config.hpp"
#include "myc/diagnostics/diagnostic_engine.hpp"
#include "myc/driver/cli_parser.hpp"
#include "myc/driver/command_dispatcher.hpp"
#include "myc/source/source_manager.hpp"

namespace myc::driver {

/// Top-level compiler driver orchestrating CLI parsing and command execution.
class CompilerDriver {
public:
    CompilerDriver();

    /// Runs the compiler driver with raw command-line arguments.
    [[nodiscard]] int Run(int argc, char* argv[]);

    [[nodiscard]] config::CompilerConfig& GetConfig();
    [[nodiscard]] const config::CompilerConfig& GetConfig() const;
    [[nodiscard]] diagnostics::DiagnosticEngine& GetDiagnostics();
    [[nodiscard]] source::SourceManager& GetSourceManager();

private:
    void LoadConfiguration(const CliOptions& options);
    void InitializeDiagnostics();

    config::CompilerConfig config_;
    diagnostics::DiagnosticEngine diagnostics_;
    source::SourceManager source_manager_;
    CommandDispatcher dispatcher_;
};

}  // namespace myc::driver
