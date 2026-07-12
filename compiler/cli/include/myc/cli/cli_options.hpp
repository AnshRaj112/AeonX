#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace myc::cli {

/// Color output preference for terminal rendering.
enum class ColorMode : std::uint8_t {
    Auto,
    Always,
    Never,
};

/// Parsed command-line interface state.
struct CliOptions {
    bool show_help{false};
    bool show_version{false};
    bool verbose{false};
    bool quiet{false};
    bool time_report{false};
    bool trace{false};
    bool diagnostics_output{false};
    bool json_output{false};

    ColorMode color_mode{ColorMode::Auto};

    std::optional<std::string> profile;
    std::optional<std::string> target;
    std::optional<std::string> platform;
    std::optional<std::string> optimization;

    std::string command;
    std::vector<std::string> positional_args;
    std::vector<std::string> source_files;
    std::map<std::string, std::string> options;
    std::vector<std::string> unknown_flags;
};

}  // namespace myc::cli
