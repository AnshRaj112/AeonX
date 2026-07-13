#pragma once

#include "myc/commands/abstract_command.hpp"

namespace myc::commands {

/// Dumps lexer token streams for one or more source files.
class TokensCommand final : public AbstractCommand {
public:
    [[nodiscard]] std::string_view GetName() const noexcept override;
    [[nodiscard]] std::string_view GetDescription() const noexcept override;
    [[nodiscard]] std::string_view GetSynopsis() const noexcept override;
    [[nodiscard]] exitcodes::ExitCode Execute(const CommandContext& ctx) const override;
};

}  // namespace myc::commands
