#include "myc/commands/version_command.hpp"

#include "myc/common/cli_output.hpp"
#include "myc/logging/logger.hpp"
#include "myc/version/version.hpp"

#include <iostream>

namespace myc::commands {

std::string_view VersionCommand::GetName() const noexcept { return "version"; }

std::string_view VersionCommand::GetDescription() const noexcept {
    return "Display compiler version information";
}

std::string_view VersionCommand::GetSynopsis() const noexcept {
    return "myc --version";
}

exitcodes::ExitCode VersionCommand::Execute(const CommandContext& ctx) const {
    logging::Logger::Instance().Info("version: displaying version information");

    if (!ctx.options.quiet) {
        common::PrintBanner(std::cout, ctx.config);
    }
    std::cout << version::GetFullVersionString() << '\n';
    return exitcodes::ExitCode::Success;
}

}  // namespace myc::commands
