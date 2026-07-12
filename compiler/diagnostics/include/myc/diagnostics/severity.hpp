#pragma once

#include <cstdint>
#include <string_view>

namespace myc::diagnostics {

/// Diagnostic severity levels ordered from most to least severe.
enum class Severity : std::uint8_t {
    Fatal,
    Error,
    Warning,
    Note,
    Hint,
    Info,
};

[[nodiscard]] constexpr std::string_view ToString(Severity severity) noexcept {
    switch (severity) {
        case Severity::Fatal:
            return "fatal";
        case Severity::Error:
            return "error";
        case Severity::Warning:
            return "warning";
        case Severity::Note:
            return "note";
        case Severity::Hint:
            return "hint";
        case Severity::Info:
            return "info";
    }
    return "unknown";
}

[[nodiscard]] constexpr bool IsErrorSeverity(Severity severity) noexcept {
    return severity == Severity::Fatal || severity == Severity::Error;
}

[[nodiscard]] constexpr bool IsWarningSeverity(Severity severity) noexcept {
    return severity == Severity::Warning;
}

}  // namespace myc::diagnostics
