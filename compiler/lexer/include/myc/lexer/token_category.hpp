#pragma once

#include <cstdint>
#include <string_view>

namespace myc::lexer {

/// High-level grouping of tokens for parser and tooling logic.
enum class TokenCategory : std::uint8_t {
    Unknown,
    Identifier,
    Keyword,
    PrimitiveType,
    Literal,
    Operator,
    Punctuation,
    Delimiter,
    Attribute,
    Annotation,
    Directive,
    Comment,
    Whitespace,
    EndOfFile,
    Error,
};

[[nodiscard]] constexpr std::string_view ToString(TokenCategory category) noexcept {
    switch (category) {
        case TokenCategory::Unknown:
            return "Unknown";
        case TokenCategory::Identifier:
            return "Identifier";
        case TokenCategory::Keyword:
            return "Keyword";
        case TokenCategory::PrimitiveType:
            return "PrimitiveType";
        case TokenCategory::Literal:
            return "Literal";
        case TokenCategory::Operator:
            return "Operator";
        case TokenCategory::Punctuation:
            return "Punctuation";
        case TokenCategory::Delimiter:
            return "Delimiter";
        case TokenCategory::Attribute:
            return "Attribute";
        case TokenCategory::Annotation:
            return "Annotation";
        case TokenCategory::Directive:
            return "Directive";
        case TokenCategory::Comment:
            return "Comment";
        case TokenCategory::Whitespace:
            return "Whitespace";
        case TokenCategory::EndOfFile:
            return "EndOfFile";
        case TokenCategory::Error:
            return "Error";
    }
    return "Unknown";
}

}  // namespace myc::lexer
