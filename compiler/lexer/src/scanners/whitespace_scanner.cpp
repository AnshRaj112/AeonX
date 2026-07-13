#include "myc/lexer/scanners/whitespace_scanner.hpp"

#include "myc/lexer/scanners/scanner_utils.hpp"
#include "myc/lexer/token_factory.hpp"

namespace myc::lexer {

namespace {

[[nodiscard]] bool IsWhitespaceChar(char32_t ch) noexcept {
    return ch == U' ' || ch == U'\t' || ch == U'\r';
}

[[nodiscard]] bool IsNewlineChar(char32_t ch) noexcept { return ch == U'\n'; }

}  // namespace

ScanResult WhitespaceScanner::Scan(LexerContext& context) {
    CharacterStream& stream = context.GetStream();
    if (stream.Eof()) {
        return ScanResult::NotHandled;
    }

    const char32_t ch = stream.Peek().GetUnicodeValue();
    if (!IsWhitespaceChar(ch) && !IsNewlineChar(ch)) {
        return ScanResult::NotHandled;
    }

    const std::size_t start = stream.Position();
    const source::SourceLocation location = stream.Peek().GetLocation();
    const bool is_newline = IsNewlineChar(ch);

    while (!stream.Eof()) {
        const char32_t current = stream.Peek().GetUnicodeValue();
        if (is_newline) {
            if (!IsNewlineChar(current)) {
                break;
            }
        } else if (!IsWhitespaceChar(current)) {
            break;
        }
        scanner_utils::AdvanceAndRecord(context);
    }

    const std::size_t end = stream.Position();
    const auto span = scanner_utils::MakeSpan(context, start, end);

    if (context.GetConfiguration().ShouldIncludeTrivia()) {
        const std::string_view lexeme = scanner_utils::SliceLexeme(context, start, end);
        const TokenType type = is_newline ? TokenType::Newline : TokenType::Whitespace;
        context.PushToken(TokenFactory::Make(type, lexeme, location, span));
        return ScanResult::TokenProduced;
    }

    return ScanResult::Consumed;
}

}  // namespace myc::lexer
