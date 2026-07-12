#include "myc/commands/command_registry.hpp"

#include "myc/commands/builtin_commands.hpp"
#include "myc/commands/help_command.hpp"
#include "myc/commands/placeholder_command.hpp"
#include "myc/commands/version_command.hpp"

#include <algorithm>

namespace myc::commands {

CommandRegistry& CommandRegistry::Instance() {
    static CommandRegistry registry;
    return registry;
}

CommandRegistry::CommandRegistry() { RegisterBuiltInCommands(); }

void CommandRegistry::Register(std::unique_ptr<AbstractCommand> command) {
    if (!command) {
        return;
    }
    const std::string name(command->GetName());
    commands_.emplace(name, std::move(command));
}

const AbstractCommand* CommandRegistry::Find(std::string_view name) const {
    const auto it = commands_.find(std::string(name));
    if (it == commands_.end()) {
        return nullptr;
    }
    return it->second.get();
}

std::vector<std::string_view> CommandRegistry::GetCommandNames() const {
    std::vector<std::string_view> names;
    names.reserve(commands_.size());
    for (const auto& [name, _] : commands_) {
        names.push_back(name);
    }
    std::sort(names.begin(), names.end());
    return names;
}

bool CommandRegistry::IsRegistered(std::string_view name) const {
    return Find(name) != nullptr;
}

void CommandRegistry::RegisterBuiltInCommands() {
    Register(std::make_unique<HelpCommand>());
    Register(std::make_unique<VersionCommand>());
    Register(std::make_unique<BuildCommand>());
    Register(std::make_unique<RunCommand>());
    Register(std::make_unique<FormatCommand>());
    Register(std::make_unique<LintCommand>());
    Register(std::make_unique<BenchmarkCommand>());

    Register(std::make_unique<PlaceholderCommand>(
        "test", "Run project unit and integration tests", "myc test [options]",
        "test runner and harness integration"));

    Register(std::make_unique<PlaceholderCommand>(
        "docs", "Generate project documentation from source annotations",
        "myc docs [options]", "documentation generator"));

    Register(std::make_unique<PlaceholderCommand>(
        "new", "Create a new Myc project from a template", "myc new <name>",
        "project scaffolding"));

    Register(std::make_unique<PlaceholderCommand>(
        "clean", "Remove build artifacts and cached outputs", "myc clean [options]",
        "build artifact management"));

    Register(std::make_unique<PlaceholderCommand>(
        "doctor", "Diagnose toolchain installation and environment configuration",
        "myc doctor", "environment diagnostics"));

    Register(std::make_unique<PlaceholderCommand>(
        "package", "Manage dependencies and package publishing", "myc package <subcommand>",
        "package manager integration"));

    Register(std::make_unique<PlaceholderCommand>(
        "init", "Initialize a myc.toml manifest in the current directory", "myc init",
        "project manifest generation"));

    Register(std::make_unique<PlaceholderCommand>(
        "repl", "Start an interactive read-eval-print loop", "myc repl",
        "interactive interpreter"));

    Register(std::make_unique<PlaceholderCommand>(
        "ast", "Dump abstract syntax trees for debugging", "myc ast <file.myc>",
        "parser and AST dumping"));

    Register(std::make_unique<PlaceholderCommand>(
        "tokens", "Dump lexer token streams for debugging", "myc tokens <file.myc>",
        "lexer token dumping"));

    Register(std::make_unique<PlaceholderCommand>(
        "ir", "Dump intermediate representation", "myc ir <file.myc>",
        "IR generation and dumping"));

    Register(std::make_unique<PlaceholderCommand>(
        "cfg", "Dump control-flow graphs for functions", "myc cfg <file.myc>",
        "CFG analysis and visualization"));
}

void EnsureCommandsRegistered() {
    (void)CommandRegistry::Instance();
}

}  // namespace myc::commands
