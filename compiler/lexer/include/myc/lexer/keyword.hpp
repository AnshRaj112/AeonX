#pragma once

#include "myc/lexer/token_type.hpp"

#include <cstdint>
#include <optional>
#include <string_view>

namespace myc::lexer {

/// Metadata for a reserved or contextual keyword.
struct Keyword {
    std::string_view text;
    TokenType type;
    bool reserved;
    std::uint32_t language_version;
    std::uint64_t feature_gate;

    [[nodiscard]] constexpr bool IsReserved() const noexcept { return reserved; }
    [[nodiscard]] constexpr bool RequiresFeature(std::uint64_t gate) const noexcept {
        return (feature_gate & gate) != 0;
    }
};

/// Centralized keyword database with efficient lookup.
class KeywordTable {
public:
    [[nodiscard]] static const Keyword* Lookup(std::string_view text) noexcept;
    [[nodiscard]] static std::optional<TokenType> LookupType(std::string_view text) noexcept;
    [[nodiscard]] static bool IsKeywordText(std::string_view text) noexcept;
    [[nodiscard]] static std::size_t Count() noexcept;
    [[nodiscard]] static const Keyword& At(std::size_t index) noexcept;
};

}  // namespace myc::lexer
