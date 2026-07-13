#include "myc/lexer/scanners/directive_scanner.hpp"

#include "myc/lexer/lexer_state.hpp"
#include "myc/lexer/scanners/scanner_utils.hpp"
#include "myc/lexer/token_factory.hpp"

namespace myc::lexer {

namespace {

[[nodiscard]] TokenType MapDirectiveName(std::string_view name) noexcept {
    if (name == "include") {
        return TokenType::DirectiveInclude;
    }
    if (name == "define") {
        return TokenType::DirectiveDefine;
    }
    if (name == "if") {
        return TokenType::DirectiveIf;
    }
    if (name == "else") {
        return TokenType::DirectiveElse;
    }
    if (name == "endif") {
        return TokenType::DirectiveEndif;
    }
    if (name == "pragma") {
        return TokenType::DirectivePragma;
    }
    return TokenType::DirectiveHash;
}

[[nodiscard]] bool IsDirectiveStart(CharacterStream& stream) {
    if (stream.Eof() || stream.Peek().GetUnicodeValue() != U'#') {
        return false;
    }

    if (stream.Peek(1).IsEof()) {
        return false;
    }

    const char32_t next = stream.Peek(1).GetUnicodeValue();
    return scanner_utils::IsIdentifierStart(next) || next == U'!';
}

}  // namespace

ScanResult DirectiveScanner::Scan(LexerContext& context) {
    if (!context.GetConfiguration().AreDirectivesEnabled()) {
        return ScanResult::NotHandled;
    }

    CharacterStream& stream = context.GetStream();
    if (!IsDirectiveStart(stream)) {
        return ScanResult::NotHandled;
    }

    const std::size_t start = stream.Position();
    const source::SourceLocation location = stream.Peek().GetLocation();

    scanner_utils::AdvanceAndRecord(context);
    context.SetState(LexerState::InsideDirective);

    const std::size_t name_start = stream.Position();
    std::size_t name_end = name_start;
    if (!stream.Eof() && scanner_utils::IsIdentifierStart(stream.Peek().GetUnicodeValue())) {
        name_end = scanner_utils::MeasureIdentifierEnd(stream);
        scanner_utils::ConsumeTo(context, name_end);
    }

    while (!stream.Eof()) {
        const char32_t ch = stream.Peek().GetUnicodeValue();
        if (ch == U'\n') {
            break;
        }
        scanner_utils::AdvanceAndRecord(context);
    }

    context.SetState(LexerState::Normal);
    const std::size_t end = stream.Position();
    const auto span = scanner_utils::MakeSpan(context, start, end);
    const std::string_view lexeme = scanner_utils::SliceLexeme(context, start, end);

    TokenType type = TokenType::DirectiveHash;
    if (name_end > name_start) {
        const std::string_view directive_name =
            scanner_utils::SliceLexeme(context, name_start, name_end);
        type = MapDirectiveName(directive_name);
    }

    context.PushToken(TokenFactory::Make(type, lexeme, location, span));
    return ScanResult::TokenProduced;
}

}  // namespace myc::lexer
