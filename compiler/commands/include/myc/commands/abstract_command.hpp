#pragma once

#include "myc/commands/command_context.hpp"
#include "myc/exitcodes/exit_codes.hpp"

#include <string_view>

namespace myc::commands {

/// Base class for all CLI commands.
class AbstractCommand {
public:
    virtual ~AbstractCommand() = default;

    [[nodiscard]] virtual std::string_view GetName() const noexcept = 0;
    [[nodiscard]] virtual std::string_view GetDescription() const noexcept = 0;
    [[nodiscard]] virtual std::string_view GetSynopsis() const noexcept = 0;

    /// Executes the command. Returns a standardized exit code.
    [[nodiscard]] virtual exitcodes::ExitCode Execute(const CommandContext& ctx) const = 0;
};

}  // namespace myc::commands
