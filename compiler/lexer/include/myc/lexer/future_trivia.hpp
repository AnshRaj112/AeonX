#pragma once

#include <cstdint>

namespace myc::lexer {

/// Placeholder for leading and trailing trivia attached to tokens.
struct FutureTrivia {
    std::uint32_t leading_count{0};
    std::uint32_t trailing_count{0};

    [[nodiscard]] constexpr bool HasLeading() const noexcept { return leading_count > 0; }
    [[nodiscard]] constexpr bool HasTrailing() const noexcept { return trailing_count > 0; }
    [[nodiscard]] constexpr bool IsEmpty() const noexcept {
        return leading_count == 0 && trailing_count == 0;
    }

    [[nodiscard]] constexpr bool operator==(const FutureTrivia& other) const noexcept = default;
};

}  // namespace myc::lexer
