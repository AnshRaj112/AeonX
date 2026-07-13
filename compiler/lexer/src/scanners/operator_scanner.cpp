#include "myc/lexer/scanners/operator_scanner.hpp"

#include "myc/lexer/scanners/scanner_utils.hpp"
#include "myc/lexer/token_factory.hpp"

namespace myc::lexer {

namespace {

[[nodiscard]] std::optional<TokenType> MapDelimiter(char32_t ch) noexcept {
    switch (ch) {
        case U'(':
            return TokenType::DelimLeftParen;
        case U')':
            return TokenType::DelimRightParen;
        case U'{':
            return TokenType::DelimLeftBrace;
        case U'}':
            return TokenType::DelimRightBrace;
        case U'[':
            return TokenType::DelimLeftBracket;
        case U']':
            return TokenType::DelimRightBracket;
        default:
            return std::nullopt;
    }
}

[[nodiscard]] std::optional<TokenType> MapPunctuation(char32_t ch) noexcept {
    switch (ch) {
        case U',':
            return TokenType::PunctComma;
        case U';':
            return TokenType::PunctSemicolon;
        case U':':
            return TokenType::PunctColon;
        case U'!':
            return TokenType::PunctBang;
        default:
            return std::nullopt;
    }
}

void PushDelimiterOrPunctuation(LexerContext& context, TokenType type, std::size_t start,
                                source::SourceLocation location) {
    CharacterStream& stream = context.GetStream();
    const std::size_t end = stream.Position();
    const auto span = scanner_utils::MakeSpan(context, start, end);
    const std::string_view lexeme = scanner_utils::SliceLexeme(context, start, end);

    if (IsDelimiter(type)) {
        context.PushToken(TokenFactory::MakeDelimiter(type, lexeme, location, span));
    } else {
        context.PushToken(TokenFactory::MakePunctuation(type, lexeme, location, span));
    }
}

}  // namespace

ScanResult OperatorScanner::Scan(LexerContext& context) {
    CharacterStream& stream = context.GetStream();
    if (stream.Eof()) {
        return ScanResult::NotHandled;
    }

    const std::size_t start = stream.Position();
    const source::SourceLocation location = stream.Peek().GetLocation();
    const char32_t ch = stream.Peek().GetUnicodeValue();

    if (ch == U'=' && !stream.Peek(1).IsEof() && stream.Peek(1).GetUnicodeValue() == U'>') {
        scanner_utils::AdvanceAndRecord(context);
        scanner_utils::AdvanceAndRecord(context);
        const std::size_t end = stream.Position();
        const auto span = scanner_utils::MakeSpan(context, start, end);
        const std::string_view lexeme = scanner_utils::SliceLexeme(context, start, end);
        context.PushToken(
            TokenFactory::MakeOperator(TokenType::OpFatArrow, lexeme, location, span));
        return ScanResult::TokenProduced;
    }

    if (const std::optional<scanner_utils::OperatorMatch> match =
            scanner_utils::MatchLongestOperator(stream)) {
        for (std::size_t i = 0; i < match->length; ++i) {
            scanner_utils::AdvanceAndRecord(context);
        }
        const std::size_t end = stream.Position();
        const auto span = scanner_utils::MakeSpan(context, start, end);
        const std::string_view lexeme = scanner_utils::SliceLexeme(context, start, end);
        context.PushToken(
            TokenFactory::MakeOperator(match->op->type, lexeme, location, span));
        return ScanResult::TokenProduced;
    }

    if (const std::optional<TokenType> delim = MapDelimiter(ch)) {
        scanner_utils::AdvanceAndRecord(context);
        PushDelimiterOrPunctuation(context, *delim, start, location);
        return ScanResult::TokenProduced;
    }

    if (const std::optional<TokenType> punct = MapPunctuation(ch)) {
        scanner_utils::AdvanceAndRecord(context);
        PushDelimiterOrPunctuation(context, *punct, start, location);
        return ScanResult::TokenProduced;
    }

    return ScanResult::NotHandled;
}

}  // namespace myc::lexer
