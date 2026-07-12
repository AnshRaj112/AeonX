#include "myc/commands/placeholder_command.hpp"

#include "myc/common/cli_output.hpp"
#include "myc/logging/logger.hpp"

#include <iostream>

namespace myc::commands {

PlaceholderCommand::PlaceholderCommand(std::string name, std::string description,
                                       std::string synopsis, std::string future_phase)
    : name_(std::move(name)),
      description_(std::move(description)),
      synopsis_(std::move(synopsis)),
      future_phase_(std::move(future_phase)) {}

std::string_view PlaceholderCommand::GetName() const noexcept { return name_; }

std::string_view PlaceholderCommand::GetDescription() const noexcept {
    return description_;
}

std::string_view PlaceholderCommand::GetSynopsis() const noexcept { return synopsis_; }

exitcodes::ExitCode PlaceholderCommand::Execute(const CommandContext& ctx) const {
    logging::Logger::Instance().Info(std::string(GetName()) + ": placeholder invoked");

    if (!ctx.options.quiet) {
        common::PrintBanner(std::cout, ctx.config);
        common::PrintPlaceholder(std::cout, GetName(), description_);
        std::cout << "Planned phase: " << future_phase_ << '\n';
    }

    return exitcodes::ExitCode::Success;
}

}  // namespace myc::commands
