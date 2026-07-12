#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace myc::driver {

/// Parsed command-line interface state.
struct CliOptions {
    bool show_help{false};
    bool show_version{false};
    std::string command;
    std::vector<std::string> positional_args;
    std::vector<std::string> source_files;
};

/// Parses raw argv into structured CLI options.
class CliParser {
public:
    [[nodiscard]] static CliOptions Parse(int argc, char* argv[]);
    [[nodiscard]] static std::string GetUsage();
};

}  // namespace myc::driver
