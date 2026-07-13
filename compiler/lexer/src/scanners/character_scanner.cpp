#include "myc/lexer/scanners/character_scanner.hpp"

#include "myc/lexer/scanners/scanner_utils.hpp"
#include "myc/lexer/token_factory.hpp"

namespace myc::lexer {

namespace {

[[nodiscard]] bool ScanCharEscape(CharacterStream& stream, LexerContext& context,
                                  std::size_t escape_start, bool& had_error) {
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
        case U'\'':
        case U'0':
            return true;
        case U'x': {
            if (stream.Eof() || !scanner_utils::IsAsciiHexDigit(stream.Peek().GetUnicodeValue())) {
                had_error = true;
                scanner_utils::PushErrorToken(context, escape_start, stream.Position(),
                                                "invalid hexadecimal escape in character literal");
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
            return true;  // Unicode placeholder.
        default:
            had_error = true;
            scanner_utils::PushErrorToken(context, escape_start, stream.Position(),
                                          "invalid escape sequence in character literal");
            return false;
    }
}

}  // namespace

ScanResult CharacterScanner::Scan(LexerContext& context) {
    CharacterStream& stream = context.GetStream();
    if (stream.Eof() || stream.Peek().GetUnicodeValue() != U'\'') {
        return ScanResult::NotHandled;
    }

    const std::size_t start = stream.Position();
    const source::SourceLocation location = stream.Peek().GetLocation();
    scanner_utils::AdvanceAndRecord(context);

    if (stream.Eof()) {
        scanner_utils::PushErrorToken(context, start, stream.Position(),
                                      "unterminated character literal");
        return ScanResult::Error;
    }

    bool had_error = false;
    if (stream.Peek().GetUnicodeValue() == U'\\') {
        const std::size_t escape_start = stream.Position();
        scanner_utils::AdvanceAndRecord(context);
        if (!ScanCharEscape(stream, context, escape_start, had_error)) {
            return ScanResult::Error;
        }
    } else if (stream.Peek().GetUnicodeValue() == U'\n') {
        scanner_utils::PushErrorToken(context, start, stream.Position(),
                                      "unterminated character literal");
        return ScanResult::Error;
    } else {
        scanner_utils::AdvanceAndRecord(context);
    }

    if (stream.Eof() || stream.Peek().GetUnicodeValue() != U'\'') {
        scanner_utils::PushErrorToken(context, start, stream.Position(),
                                      "unterminated character literal");
        return ScanResult::Error;
    }

    scanner_utils::AdvanceAndRecord(context);
    const std::size_t end = stream.Position();
    const auto span = scanner_utils::MakeSpan(context, start, end);
    const std::string_view lexeme = scanner_utils::SliceLexeme(context, start, end);

    TokenFlags flags = TokenFlags::None;
    if (had_error) {
        flags = TokenFlags::Invalid | TokenFlags::Recoverable;
    }

    context.PushToken(TokenFactory::MakeLiteral(TokenType::CharacterLiteral, LiteralKind::Character,
                                                lexeme, location, span, flags));
    return had_error ? ScanResult::Error : ScanResult::TokenProduced;
}

}  // namespace myc::lexer
