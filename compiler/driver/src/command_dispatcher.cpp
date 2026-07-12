#include "myc/driver/command_dispatcher.hpp"

#include "myc/commands/command_registry.hpp"
#include "myc/exitcodes/exit_codes.hpp"
#include "myc/logging/logger.hpp"

#include <iostream>

namespace myc::driver {

CommandDispatcher::CommandDispatcher(config::CompilerConfig& config,
                                     diagnostics::DiagnosticEngine& diagnostics,
                                     source::SourceManager& source_manager)
    : config_(config), diagnostics_(diagnostics), source_manager_(source_manager) {
    commands::EnsureCommandsRegistered();
}

int CommandDispatcher::Dispatch(const cli::CliOptions& options) {
    if (options.show_help) {
        if (const commands::AbstractCommand* help =
                commands::CommandRegistry::Instance().Find("help")) {
            return ExecuteCommand(*help, options);
        }
    }

    if (options.show_version) {
        if (const commands::AbstractCommand* version =
                commands::CommandRegistry::Instance().Find("version")) {
            return ExecuteCommand(*version, options);
        }
    }

    if (options.command.empty()) {
        diagnostics_.EmitError("D0002", "no command specified; run 'myc --help' for usage");
        return exitcodes::ToInt(exitcodes::ExitCode::InvalidArguments);
    }

    const commands::AbstractCommand* command =
        commands::CommandRegistry::Instance().Find(options.command);
    if (!command) {
        diagnostics_.EmitError("D0003",
                               "unknown command '" + options.command +
                                   "'; run 'myc --help' for usage");
        return exitcodes::ToInt(exitcodes::ExitCode::InvalidArguments);
    }

    return ExecuteCommand(*command, options);
}

int CommandDispatcher::ExecuteCommand(const commands::AbstractCommand& command,
                                      const cli::CliOptions& options) {
    logging::Logger::Instance().Info("dispatching command: " + std::string(command.GetName()));

    const commands::CommandContext ctx{config_, diagnostics_, source_manager_, options};
    const exitcodes::ExitCode exit_code = command.Execute(ctx);

    if (exit_code != exitcodes::ExitCode::Success) {
        return exitcodes::ToInt(exit_code);
    }

    if (diagnostics_.HasErrors()) {
        return exitcodes::ToInt(exitcodes::ExitCode::GeneralError);
    }

    return exitcodes::ToInt(exitcodes::ExitCode::Success);
}

}  // namespace myc::driver
