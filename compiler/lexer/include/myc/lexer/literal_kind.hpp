#pragma once

#include <cstdint>
#include <string_view>

namespace myc::lexer {

/// Discriminates literal tokens for future semantic analysis.
enum class LiteralKind : std::uint8_t {
    None,
    Integer,
    Float,
    Hexadecimal,
    Binary,
    Octal,
    Scientific,
    Boolean,
    Character,
    String,
    RawString,
    MultilineString,
    InterpolatedString,  // future
};

[[nodiscard]] constexpr std::string_view ToString(LiteralKind kind) noexcept {
    switch (kind) {
        case LiteralKind::None:
            return "None";
        case LiteralKind::Integer:
            return "Integer";
        case LiteralKind::Float:
            return "Float";
        case LiteralKind::Hexadecimal:
            return "Hexadecimal";
        case LiteralKind::Binary:
            return "Binary";
        case LiteralKind::Octal:
            return "Octal";
        case LiteralKind::Scientific:
            return "Scientific";
        case LiteralKind::Boolean:
            return "Boolean";
        case LiteralKind::Character:
            return "Character";
        case LiteralKind::String:
            return "String";
        case LiteralKind::RawString:
            return "RawString";
        case LiteralKind::MultilineString:
            return "MultilineString";
        case LiteralKind::InterpolatedString:
            return "InterpolatedString";
    }
    return "None";
}

[[nodiscard]] constexpr bool IsNumericLiteral(LiteralKind kind) noexcept {
    switch (kind) {
        case LiteralKind::Integer:
        case LiteralKind::Float:
        case LiteralKind::Hexadecimal:
        case LiteralKind::Binary:
        case LiteralKind::Octal:
        case LiteralKind::Scientific:
            return true;
        default:
            return false;
    }
}

}  // namespace myc::lexer
