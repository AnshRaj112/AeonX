#pragma once

#include <cstdint>

namespace myc::lexer {

/// Placeholder for extended Unicode position metadata on characters.
///
/// Future IDE and diagnostic features may require UTF-16 column offsets (for
/// JavaScript interop), UTF-32 code-unit columns (for grapheme-aware editing),
/// and grapheme-cluster indices. These fields are reserved but not populated yet.
struct FutureUnicodeSupport {
    std::uint32_t utf16_column{0};
    std::uint32_t utf32_column{0};
    std::uint32_t grapheme_index{0};

    [[nodiscard]] constexpr bool operator==(const FutureUnicodeSupport& other) const noexcept =
        default;
};

}  // namespace myc::lexer
