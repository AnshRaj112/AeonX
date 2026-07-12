#include "myc/common/cli_output.hpp"

#include "myc/version/version.hpp"

#include <iostream>

namespace myc::common {

namespace {

std::string_view FormatProfile(config::BuildProfile profile) noexcept {
    switch (profile) {
        case config::BuildProfile::Debug:
            return "Debug";
        case config::BuildProfile::Release:
            return "Release";
        case config::BuildProfile::RelWithDebInfo:
            return "RelWithDebInfo";
        case config::BuildProfile::MinSizeRel:
            return "MinSizeRel";
        case config::BuildProfile::Custom:
            return "Custom";
    }
    return "Unknown";
}

std::string_view FormatLlvmBackend() noexcept {
    if (version::kLlvmVersion == "not-linked") {
        return "Not Yet Enabled";
    }
    return version::kLlvmVersion;
}

}  // namespace

void PrintBanner(std::ostream& stream, const config::CompilerConfig& config) {
    const auto arch = config.GetTargetArchitecture();
    const std::string_view target =
        arch == config::TargetArchitecture::Unknown ? "host-default"
                                                    : ToString(arch);

    stream << "----------------------------------------\n"
           << version::kLanguageName << " Compiler " << version::kCompilerVersion << '\n'
           << "Target: " << target << '\n'
           << "LLVM Backend: " << FormatLlvmBackend() << '\n'
           << "Profile: " << FormatProfile(config.GetBuildProfile()) << '\n'
           << "----------------------------------------\n";
}

void PrintPlaceholder(std::ostream& stream, std::string_view command_name,
                      std::string_view description) {
    stream << '[' << command_name << "] " << description << '\n'
           << "This command is registered but not yet implemented.\n";
}

void PrintCommandHelp(std::ostream& stream, std::string_view command_name,
                      std::string_view synopsis, std::string_view description) {
    stream << "NAME\n"
           << "    " << command_name << '\n'
           << '\n'
           << "SYNOPSIS\n"
           << "    " << synopsis << '\n'
           << '\n'
           << "DESCRIPTION\n"
           << "    " << description << '\n';
}

}  // namespace myc::common
