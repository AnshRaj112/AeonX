#include "myc/lexer/scanners/number_scanner.hpp"

#include "myc/lexer/scanners/scanner_utils.hpp"
#include "myc/lexer/token_factory.hpp"

namespace myc::lexer {

namespace {

[[nodiscard]] bool IsDecimalStart(char32_t ch) noexcept {
    return scanner_utils::IsAsciiDigit(ch);
}

[[nodiscard]] bool ConsumeDigits(CharacterStream& stream, LexerContext& context,
                                 bool (*digit_pred)(char32_t)) {
    bool consumed = false;
    while (!stream.Eof() && digit_pred(stream.Peek().GetUnicodeValue())) {
        scanner_utils::AdvanceAndRecord(context);
        consumed = true;
    }
    return consumed;
}

[[nodiscard]] bool ConsumeDecimalDigits(CharacterStream& stream, LexerContext& context) {
    return ConsumeDigits(stream, context, scanner_utils::IsAsciiDigit);
}

[[nodiscard]] ScanResult EmitNumber(LexerContext& context, std::size_t start,
                                    source::SourceLocation location, LiteralKind kind,
                                    bool is_float, bool had_error) {
    CharacterStream& stream = context.GetStream();
    const std::size_t end = stream.Position();
    const auto span = scanner_utils::MakeSpan(context, start, end);
    const std::string_view lexeme = scanner_utils::SliceLexeme(context, start, end);

    const TokenType token_type = is_float ? TokenType::FloatLiteral : TokenType::IntegerLiteral;
    TokenFlags flags = TokenFlags::None;
    if (had_error) {
        flags = TokenFlags::Invalid | TokenFlags::Recoverable;
    }

    context.PushToken(
        TokenFactory::MakeLiteral(token_type, kind, lexeme, location, span, flags));
    return had_error ? ScanResult::Error : ScanResult::TokenProduced;
}

}  // namespace

ScanResult NumberScanner::Scan(LexerContext& context) {
    CharacterStream& stream = context.GetStream();
    if (stream.Eof()) {
        return ScanResult::NotHandled;
    }

    const char32_t ch = stream.Peek().GetUnicodeValue();
    if (!IsDecimalStart(ch) && ch != U'.') {
        return ScanResult::NotHandled;
    }

    if (ch == U'.') {
        if (stream.Peek(1).IsEof() ||
            !scanner_utils::IsAsciiDigit(stream.Peek(1).GetUnicodeValue())) {
            return ScanResult::NotHandled;
        }
    }

    const std::size_t start = stream.Position();
    const source::SourceLocation location = stream.Peek().GetLocation();
    bool had_error = false;
    bool is_float = false;
    LiteralKind kind = LiteralKind::Integer;

    if (ch == U'0' && !stream.Peek(1).IsEof()) {
        const char32_t prefix = stream.Peek(1).GetUnicodeValue();
        if (prefix == U'x' || prefix == U'X') {
            scanner_utils::AdvanceAndRecord(context);
            scanner_utils::AdvanceAndRecord(context);
            kind = LiteralKind::Hexadecimal;
            if (!ConsumeDigits(stream, context, scanner_utils::IsAsciiHexDigit)) {
                had_error = true;
                scanner_utils::PushErrorToken(context, start, stream.Position(),
                                              "invalid hexadecimal number literal");
                return ScanResult::Error;
            }
            return EmitNumber(context, start, location, kind, false, had_error);
        }
        if (prefix == U'b' || prefix == U'B') {
            scanner_utils::AdvanceAndRecord(context);
            scanner_utils::AdvanceAndRecord(context);
            kind = LiteralKind::Binary;
            if (!ConsumeDigits(stream, context, [](char32_t c) { return c == U'0' || c == U'1'; })) {
                had_error = true;
                scanner_utils::PushErrorToken(context, start, stream.Position(),
                                              "invalid binary number literal");
                return ScanResult::Error;
            }
            return EmitNumber(context, start, location, kind, false, had_error);
        }
        if (prefix == U'o' || prefix == U'O') {
            scanner_utils::AdvanceAndRecord(context);
            scanner_utils::AdvanceAndRecord(context);
            kind = LiteralKind::Octal;
            if (!ConsumeDigits(stream, context, scanner_utils::IsAsciiOctDigit)) {
                had_error = true;
                scanner_utils::PushErrorToken(context, start, stream.Position(),
                                              "invalid octal number literal");
                return ScanResult::Error;
            }
            return EmitNumber(context, start, location, kind, false, had_error);
        }
    }

    if (ch != U'.') {
        (void)ConsumeDecimalDigits(stream, context);
    }

    if (!stream.Eof() && stream.Peek().GetUnicodeValue() == U'.') {
        if (!stream.Peek(1).IsEof() &&
            scanner_utils::IsAsciiDigit(stream.Peek(1).GetUnicodeValue())) {
            is_float = true;
            kind = LiteralKind::Float;
            scanner_utils::AdvanceAndRecord(context);
            (void)ConsumeDecimalDigits(stream, context);
        }
    }

    if (!stream.Eof()) {
        const char32_t exp_ch = stream.Peek().GetUnicodeValue();
        if (exp_ch == U'e' || exp_ch == U'E') {
            is_float = true;
            kind = LiteralKind::Scientific;
            scanner_utils::AdvanceAndRecord(context);
            if (!stream.Eof()) {
                const char32_t sign = stream.Peek().GetUnicodeValue();
                if (sign == U'+' || sign == U'-') {
                    scanner_utils::AdvanceAndRecord(context);
                }
            }
            if (!ConsumeDecimalDigits(stream, context)) {
                had_error = true;
                scanner_utils::PushErrorToken(context, start, stream.Position(),
                                              "invalid exponent in number literal");
                return ScanResult::Error;
            }
        }
    }

    if (!stream.Eof() && scanner_utils::IsIdentifierStart(stream.Peek().GetUnicodeValue())) {
        had_error = true;
        scanner_utils::PushErrorToken(context, start, stream.Position(),
                                      "invalid suffix on number literal");
        return ScanResult::Error;
    }

    return EmitNumber(context, start, location, kind, is_float, had_error);
}

}  // namespace myc::lexer
