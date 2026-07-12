#pragma once

#include "myc/diagnostics/source_location.hpp"
#include "myc/types.hpp"

#include <cstddef>
#include <optional>

namespace myc::diagnostics {

/// A half-open byte range [start_offset, end_offset) with location metadata.
struct SourceSpan {
    std::optional<FileId> file_id;
    SourceLocation start;
    SourceLocation end;
    std::size_t start_offset{0};
    std::size_t end_offset{0};
    std::size_t source_length{0};

    [[nodiscard]] bool IsValid() const noexcept {
        return start.HasFilename() || file_id.has_value();
    }

    [[nodiscard]] bool IsMultiLine() const noexcept { return start.line != end.line; }

    [[nodiscard]] bool operator==(const SourceSpan& other) const = default;
};

}  // namespace myc::diagnostics
