#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace myc {

/// Common type aliases used across the toolchain.
using Index = std::int32_t;
using LineNumber = std::uint32_t;
using ColumnNumber = std::uint32_t;
using FileId = std::uint32_t;

/// Represents a half-open source range [start, end).
struct SourceRange {
    FileId file_id{0};
    LineNumber start_line{1};
    ColumnNumber start_column{1};
    LineNumber end_line{1};
    ColumnNumber end_column{1};
};

/// Severity levels shared between diagnostics and logging.
enum class Severity : std::uint8_t {
    Note,
    Hint,
    Warning,
    Error,
};

[[nodiscard]] constexpr std::string_view ToString(Severity severity) noexcept {
    switch (severity) {
        case Severity::Note:
            return "note";
        case Severity::Hint:
            return "hint";
        case Severity::Warning:
            return "warning";
        case Severity::Error:
            return "error";
    }
    return "unknown";
}

}  // namespace myc
