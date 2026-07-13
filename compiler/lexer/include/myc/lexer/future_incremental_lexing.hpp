#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

namespace myc::lexer {

/// Placeholder describing a region of source that changed and may be re-lexed.
struct IncrementalEditRegion {
    std::size_t start_offset{0};
    std::size_t old_length{0};
    std::size_t new_length{0};

    [[nodiscard]] constexpr bool operator==(const IncrementalEditRegion& other) const noexcept {
        return start_offset == other.start_offset && old_length == other.old_length &&
               new_length == other.new_length;
    }
};

/// Placeholder for future IDE incremental lexing support.
///
/// When implemented, the lexer will re-tokenize only modified regions and
/// splice the result into an existing token vector, avoiding full-file rescans.
struct FutureIncrementalLexing {
    std::uint32_t generation{0};
    std::optional<IncrementalEditRegion> pending_edit;

    [[nodiscard]] constexpr bool HasPendingEdit() const noexcept {
        return pending_edit.has_value();
    }

    [[nodiscard]] bool operator==(const FutureIncrementalLexing& other) const noexcept {
        return generation == other.generation && pending_edit == other.pending_edit;
    }
};

}  // namespace myc::lexer
