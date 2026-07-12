#pragma once

#include <cstdint>

namespace myc::lexer {

/// Placeholder for macro expansion provenance on tokens.
struct FutureMacroToken {
    std::uint32_t expansion_depth{0};
    std::uint32_t macro_definition_id{0};

    [[nodiscard]] constexpr bool IsFromMacro() const noexcept { return expansion_depth > 0; }

    [[nodiscard]] constexpr bool operator==(const FutureMacroToken& other) const noexcept = default;
};

/// Placeholder for parsed literal or identifier semantic values.
struct FutureSemanticValue {
    std::uint64_t payload{0};

    [[nodiscard]] constexpr bool HasValue() const noexcept { return payload != 0; }

    [[nodiscard]] constexpr bool operator==(const FutureSemanticValue& other) const noexcept =
        default;
};

}  // namespace myc::lexer
