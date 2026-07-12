#pragma once

#include "myc/commands/abstract_command.hpp"

#include <string>

namespace myc::commands {

/// Placeholder command for features not yet implemented.
class PlaceholderCommand final : public AbstractCommand {
public:
    PlaceholderCommand(std::string name, std::string description, std::string synopsis,
                       std::string future_phase);

    [[nodiscard]] std::string_view GetName() const noexcept override;
    [[nodiscard]] std::string_view GetDescription() const noexcept override;
    [[nodiscard]] std::string_view GetSynopsis() const noexcept override;

    [[nodiscard]] exitcodes::ExitCode Execute(const CommandContext& ctx) const override;

private:
    std::string name_;
    std::string description_;
    std::string synopsis_;
    std::string future_phase_;
};

}  // namespace myc::commands
