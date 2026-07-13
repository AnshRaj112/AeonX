#pragma once

#include "myc/lexer/lexer_context.hpp"
#include "myc/lexer/operator.hpp"

#include <cstddef>
#include <optional>
#include <string_view>

namespace myc::lexer::scanner_utils {

[[nodiscard]] constexpr bool IsAsciiDigit(char32_t ch) noexcept {
    return ch >= U'0' && ch <= U'9';
}

[[nodiscard]] constexpr bool IsAsciiHexDigit(char32_t ch) noexcept {
    return IsAsciiDigit(ch) || (ch >= U'a' && ch <= U'f') || (ch >= U'A' && ch <= U'F');
}

[[nodiscard]] constexpr bool IsAsciiOctDigit(char32_t ch) noexcept {
    return ch >= U'0' && ch <= U'7';
}

[[nodiscard]] constexpr bool IsAsciiAlpha(char32_t ch) noexcept {
    return (ch >= U'a' && ch <= U'z') || (ch >= U'A' && ch <= U'Z');
}

/// Identifier start: ASCII letters, underscore, or Unicode placeholder (non-ASCII non-digit).
[[nodiscard]] bool IsIdentifierStart(char32_t ch) noexcept;

/// Identifier continuation: start chars plus ASCII digits.
[[nodiscard]] bool IsIdentifierContinue(char32_t ch) noexcept;

void AdvanceAndRecord(LexerContext& context);

void RecordAdvance(LexerContext& context, std::size_t code_points = 1);

[[nodiscard]] source::SourceSpan MakeSpan(LexerContext& context, std::size_t start,
                                          std::size_t end);

/// Advances the stream and records statistics while consuming an identifier body.
[[nodiscard]] std::size_t ScanIdentifierEnd(LexerContext& context);

/// Returns the byte offset after a identifier body without advancing the stream.
[[nodiscard]] std::size_t MeasureIdentifierEnd(const CharacterStream& stream);

void ConsumeTo(LexerContext& context, std::size_t end);

[[nodiscard]] std::string_view SliceLexeme(LexerContext& context, std::size_t start,
                                           std::size_t end);

struct OperatorMatch {
    const Operator* op{nullptr};
    std::size_t length{0};
};

[[nodiscard]] std::optional<OperatorMatch> MatchLongestOperator(CharacterStream& stream);

void EmitLexicalError(LexerContext& context, std::size_t start, std::size_t end,
                      std::string_view message);

void PushErrorToken(LexerContext& context, std::size_t start, std::size_t end,
                    std::string_view message);

}  // namespace myc::lexer::scanner_utils
