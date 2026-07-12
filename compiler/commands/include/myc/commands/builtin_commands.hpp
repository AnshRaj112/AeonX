#pragma once

#include "myc/commands/abstract_command.hpp"

namespace myc::commands {

class BuildCommand final : public AbstractCommand {
public:
    [[nodiscard]] std::string_view GetName() const noexcept override;
    [[nodiscard]] std::string_view GetDescription() const noexcept override;
    [[nodiscard]] std::string_view GetSynopsis() const noexcept override;
    [[nodiscard]] exitcodes::ExitCode Execute(const CommandContext& ctx) const override;
};

class RunCommand final : public AbstractCommand {
public:
    [[nodiscard]] std::string_view GetName() const noexcept override;
    [[nodiscard]] std::string_view GetDescription() const noexcept override;
    [[nodiscard]] std::string_view GetSynopsis() const noexcept override;
    [[nodiscard]] exitcodes::ExitCode Execute(const CommandContext& ctx) const override;
};

class FormatCommand final : public AbstractCommand {
public:
    [[nodiscard]] std::string_view GetName() const noexcept override;
    [[nodiscard]] std::string_view GetDescription() const noexcept override;
    [[nodiscard]] std::string_view GetSynopsis() const noexcept override;
    [[nodiscard]] exitcodes::ExitCode Execute(const CommandContext& ctx) const override;
};

class LintCommand final : public AbstractCommand {
public:
    [[nodiscard]] std::string_view GetName() const noexcept override;
    [[nodiscard]] std::string_view GetDescription() const noexcept override;
    [[nodiscard]] std::string_view GetSynopsis() const noexcept override;
    [[nodiscard]] exitcodes::ExitCode Execute(const CommandContext& ctx) const override;
};

class BenchmarkCommand final : public AbstractCommand {
public:
    [[nodiscard]] std::string_view GetName() const noexcept override;
    [[nodiscard]] std::string_view GetDescription() const noexcept override;
    [[nodiscard]] std::string_view GetSynopsis() const noexcept override;
    [[nodiscard]] exitcodes::ExitCode Execute(const CommandContext& ctx) const override;
};

}  // namespace myc::commands
