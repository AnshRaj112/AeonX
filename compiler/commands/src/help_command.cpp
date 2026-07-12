#include "myc/commands/help_command.hpp"

#include "myc/cli/cli_parser.hpp"
#include "myc/commands/command_registry.hpp"
#include "myc/common/cli_output.hpp"
#include "myc/logging/logger.hpp"

#include <iostream>

namespace myc::commands {

std::string_view HelpCommand::GetName() const noexcept { return "help"; }

std::string_view HelpCommand::GetDescription() const noexcept {
    return "Display help information for commands";
}

std::string_view HelpCommand::GetSynopsis() const noexcept {
    return "myc help [command]";
}

exitcodes::ExitCode HelpCommand::Execute(const CommandContext& ctx) const {
    logging::Logger::Instance().Info("help: displaying usage information");

    if (!ctx.options.positional_args.empty()) {
        const std::string_view topic = ctx.options.positional_args.front();
        if (const AbstractCommand* command = CommandRegistry::Instance().Find(topic)) {
            common::PrintCommandHelp(std::cout, command->GetName(), command->GetSynopsis(),
                                     command->GetDescription());
            return exitcodes::ExitCode::Success;
        }
        std::cerr << "error: no help available for '" << topic << "'\n";
        return exitcodes::ExitCode::InvalidArguments;
    }

    std::cout << cli::CliParser::GetUsage() << '\n';
    return exitcodes::ExitCode::Success;
}

}  // namespace myc::commands
