#include "myc/lexer/scanners/comment_scanner.hpp"

#include "myc/lexer/lexer_state.hpp"
#include "myc/lexer/scanners/scanner_utils.hpp"
#include "myc/lexer/token_factory.hpp"

namespace myc::lexer {

ScanResult CommentScanner::Scan(LexerContext& context) {
    CharacterStream& stream = context.GetStream();
    if (stream.Eof()) {
        return ScanResult::NotHandled;
    }

    if (stream.Peek().GetUnicodeValue() != U'/') {
        return ScanResult::NotHandled;
    }
    if (stream.Peek(1).IsEof()) {
        return ScanResult::NotHandled;
    }

    const char32_t next = stream.Peek(1).GetUnicodeValue();
    if (next != U'/' && next != U'*') {
        return ScanResult::NotHandled;
    }

    const std::size_t start = stream.Position();
    const source::SourceLocation location = stream.Peek().GetLocation();
    const bool is_line_comment = next == U'/';

    scanner_utils::AdvanceAndRecord(context);
    scanner_utils::AdvanceAndRecord(context);

    context.SetState(LexerState::InsideComment);

    if (is_line_comment) {
        while (!stream.Eof()) {
            const char32_t ch = stream.Peek().GetUnicodeValue();
            if (ch == U'\n') {
                break;
            }
            scanner_utils::AdvanceAndRecord(context);
        }
    } else {
        int depth = 1;
        while (!stream.Eof() && depth > 0) {
            const char32_t ch = stream.Peek().GetUnicodeValue();
            if (ch == U'/' && stream.Peek(1).GetUnicodeValue() == U'*') {
                scanner_utils::AdvanceAndRecord(context);
                scanner_utils::AdvanceAndRecord(context);
                ++depth;
                continue;
            }
            if (ch == U'*' && stream.Peek(1).GetUnicodeValue() == U'/') {
                scanner_utils::AdvanceAndRecord(context);
                scanner_utils::AdvanceAndRecord(context);
                --depth;
                continue;
            }
            scanner_utils::AdvanceAndRecord(context);
        }

        if (depth > 0) {
            scanner_utils::PushErrorToken(context, start, stream.Position(),
                                          "unterminated block comment");
            context.SetState(LexerState::Normal);
            return ScanResult::Error;
        }
    }

    context.SetState(LexerState::Normal);
    const std::size_t end = stream.Position();
    const auto span = scanner_utils::MakeSpan(context, start, end);

    if (context.GetConfiguration().ShouldIncludeTrivia()) {
        const std::string_view lexeme = scanner_utils::SliceLexeme(context, start, end);
        context.PushToken(TokenFactory::Make(TokenType::Comment, lexeme, location, span));
        return ScanResult::TokenProduced;
    }

    return ScanResult::Consumed;
}

}  // namespace myc::lexer
