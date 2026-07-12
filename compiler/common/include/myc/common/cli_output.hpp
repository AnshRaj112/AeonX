#pragma once

#include "myc/config/compiler_config.hpp"

#include <iosfwd>
#include <string_view>

namespace myc::common {

/// Renders the standard compiler status banner.
void PrintBanner(std::ostream& stream, const config::CompilerConfig& config);

/// Renders a consistent placeholder message for unimplemented commands.
void PrintPlaceholder(std::ostream& stream, std::string_view command_name,
                      std::string_view description);

/// Renders command-specific help text.
void PrintCommandHelp(std::ostream& stream, std::string_view command_name,
                      std::string_view synopsis, std::string_view description);

}  // namespace myc::common
