#pragma once

#include <cstdint>
#include <type_traits>

namespace myc::lexer {

/// Bit flags attached to tokens for lexer and parser metadata.
enum class TokenFlags : std::uint32_t {
    None = 0,
    HasLeadingTrivia = 1u << 0,
    HasTrailingTrivia = 1u << 1,
    FromMacroExpansion = 1u << 2,
    Invalid = 1u << 3,
    Synthetic = 1u << 4,
    Recoverable = 1u << 5,
    AtStartOfLine = 1u << 6,
    PrecededByWhitespace = 1u << 7,
};

[[nodiscard]] constexpr TokenFlags operator|(TokenFlags lhs, TokenFlags rhs) noexcept {
    using Underlying = std::underlying_type_t<TokenFlags>;
    return static_cast<TokenFlags>(static_cast<Underlying>(lhs) |
                                   static_cast<Underlying>(rhs));
}

[[nodiscard]] constexpr TokenFlags operator&(TokenFlags lhs, TokenFlags rhs) noexcept {
    using Underlying = std::underlying_type_t<TokenFlags>;
    return static_cast<TokenFlags>(static_cast<Underlying>(lhs) &
                                   static_cast<Underlying>(rhs));
}

[[nodiscard]] constexpr TokenFlags operator~(TokenFlags flags) noexcept {
    using Underlying = std::underlying_type_t<TokenFlags>;
    return static_cast<TokenFlags>(~static_cast<Underlying>(flags));
}

[[nodiscard]] constexpr bool HasFlag(TokenFlags flags, TokenFlags flag) noexcept {
    return (flags & flag) != TokenFlags::None;
}

[[nodiscard]] constexpr TokenFlags& operator|=(TokenFlags& lhs, TokenFlags rhs) noexcept {
    lhs = lhs | rhs;
    return lhs;
}

[[nodiscard]] constexpr TokenFlags& operator&=(TokenFlags& lhs, TokenFlags rhs) noexcept {
    lhs = lhs & rhs;
    return lhs;
}

}  // namespace myc::lexer
