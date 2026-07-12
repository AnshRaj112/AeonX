#pragma once

#include "myc/commands/abstract_command.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace myc::commands {

/// Factory and registry for CLI commands.
class CommandRegistry {
public:
    static CommandRegistry& Instance();

    void Register(std::unique_ptr<AbstractCommand> command);
    [[nodiscard]] const AbstractCommand* Find(std::string_view name) const;
    [[nodiscard]] std::vector<std::string_view> GetCommandNames() const;
    [[nodiscard]] bool IsRegistered(std::string_view name) const;

private:
    CommandRegistry();
    void RegisterBuiltInCommands();

    std::unordered_map<std::string, std::unique_ptr<AbstractCommand>> commands_;
};

/// Ensures all built-in commands are registered. Called during driver startup.
void EnsureCommandsRegistered();

}  // namespace myc::commands
