#include "myc/lexer/scanners/string_scanner.hpp"

#include "myc/lexer/lexer_state.hpp"
#include "myc/lexer/scanners/scanner_utils.hpp"
#include "myc/lexer/token_factory.hpp"

namespace myc::lexer {

namespace {

[[nodiscard]] bool ScanEscape(CharacterStream& stream, LexerContext& context, std::size_t escape_start,
                              bool& had_error) {
    if (stream.Eof()) {
        had_error = true;
        return false;
    }

    const char32_t esc = stream.Peek().GetUnicodeValue();
    scanner_utils::AdvanceAndRecord(context);

    switch (esc) {
        case U'n':
        case U't':
        case U'r':
        case U'\\':
        case U'"':
        case U'\'':
        case U'0':
            return true;
        case U'x': {
            if (stream.Eof()) {
                had_error = true;
                return false;
            }
            if (!scanner_utils::IsAsciiHexDigit(stream.Peek().GetUnicodeValue())) {
                had_error = true;
                scanner_utils::PushErrorToken(context, escape_start, stream.Position(),
                                              "invalid hexadecimal escape in string");
                return false;
            }
            scanner_utils::AdvanceAndRecord(context);
            if (!stream.Eof() && scanner_utils::IsAsciiHexDigit(stream.Peek().GetUnicodeValue())) {
                scanner_utils::AdvanceAndRecord(context);
            }
            return true;
        }
        case U'u':
        case U'U':
            // Unicode escape placeholder.
            return true;
        default:
            had_error = true;
            scanner_utils::PushErrorToken(context, escape_start, stream.Position(),
                                          "invalid escape sequence in string");
            return false;
    }
}

}  // namespace

ScanResult StringScanner::Scan(LexerContext& context) {
    CharacterStream& stream = context.GetStream();
    if (stream.Eof()) {
        return ScanResult::NotHandled;
    }

    const char32_t ch = stream.Peek().GetUnicodeValue();
    if (ch != U'"') {
        if (ch == U'r' && !stream.Peek(1).IsEof() && stream.Peek(1).GetUnicodeValue() == U'"') {
            return ScanResult::NotHandled;  // Raw string placeholder handled elsewhere.
        }
        if (ch == U'"' && stream.Peek(1).GetUnicodeValue() == U'"' &&
            stream.Peek(2).GetUnicodeValue() == U'"') {
            return ScanResult::NotHandled;  // Multiline placeholder.
        }
        return ScanResult::NotHandled;
    }

    const std::size_t start = stream.Position();
    const source::SourceLocation location = stream.Peek().GetLocation();

    scanner_utils::AdvanceAndRecord(context);
    context.SetState(LexerState::InsideString);

    bool had_error = false;
    while (!stream.Eof()) {
        const char32_t current = stream.Peek().GetUnicodeValue();
        if (current == U'"') {
            scanner_utils::AdvanceAndRecord(context);
            context.SetState(LexerState::Normal);
            const std::size_t end = stream.Position();
            const auto span = scanner_utils::MakeSpan(context, start, end);
            const std::string_view lexeme = scanner_utils::SliceLexeme(context, start, end);
            TokenFlags flags = TokenFlags::None;
            if (had_error) {
                flags = TokenFlags::Invalid | TokenFlags::Recoverable;
            }
            context.PushToken(TokenFactory::MakeLiteral(TokenType::StringLiteral,
                                                        LiteralKind::String, lexeme, location,
                                                        span, flags));
            return had_error ? ScanResult::Error : ScanResult::TokenProduced;
        }

        if (current == U'\\') {
            const std::size_t escape_start = stream.Position();
            scanner_utils::AdvanceAndRecord(context);
            if (!ScanEscape(stream, context, escape_start, had_error)) {
                if (!had_error) {
                    break;
                }
            }
            continue;
        }

        if (current == U'\n') {
            had_error = true;
            scanner_utils::PushErrorToken(context, start, stream.Position(),
                                          "unterminated string literal");
            context.SetState(LexerState::Normal);
            return ScanResult::Error;
        }

        scanner_utils::AdvanceAndRecord(context);
    }

    had_error = true;
    scanner_utils::PushErrorToken(context, start, stream.Position(), "unterminated string literal");
    context.SetState(LexerState::Normal);
    return ScanResult::Error;
}

}  // namespace myc::lexer
