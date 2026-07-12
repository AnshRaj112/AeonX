#pragma once

#include "myc/cli/cli_options.hpp"
#include "myc/common/result.hpp"

#include <string>

namespace myc::cli {

/// Parses raw argv into structured CLI options.
class CliParser {
public:
    [[nodiscard]] static common::Result<CliOptions> Parse(int argc, char* argv[]);
    [[nodiscard]] static std::string GetUsage();
    [[nodiscard]] static std::string GetVersionUsage();
};

}  // namespace myc::cli
