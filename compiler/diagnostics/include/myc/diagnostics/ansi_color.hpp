#pragma once

#include "myc/diagnostics/severity.hpp"

#include <string>
#include <string_view>

namespace myc::diagnostics {

/// ANSI escape sequences for terminal color output.
namespace ansi {

inline constexpr std::string_view Reset = "\033[0m";
inline constexpr std::string_view Bold = "\033[1m";
inline constexpr std::string_view Red = "\033[31m";
inline constexpr std::string_view Yellow = "\033[33m";
inline constexpr std::string_view Blue = "\033[34m";
inline constexpr std::string_view Cyan = "\033[36m";
inline constexpr std::string_view Gray = "\033[90m";

[[nodiscard]] std::string Colorize(std::string_view text, std::string_view color,
                                   bool enabled);

[[nodiscard]] std::string_view SeverityColor(Severity severity) noexcept;

}  // namespace ansi

}  // namespace myc::diagnostics
