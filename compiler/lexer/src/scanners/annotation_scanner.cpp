#include "myc/lexer/scanners/annotation_scanner.hpp"

#include "myc/lexer/scanners/scanner_utils.hpp"
#include "myc/lexer/token_factory.hpp"

namespace myc::lexer {

ScanResult AnnotationScanner::Scan(LexerContext& context) {
    if (!context.GetConfiguration().AreAnnotationsEnabled()) {
        return ScanResult::NotHandled;
    }

    CharacterStream& stream = context.GetStream();
    if (stream.Eof() || stream.Peek().GetUnicodeValue() != U'@') {
        return ScanResult::NotHandled;
    }

    if (stream.Peek(1).IsEof() ||
        !scanner_utils::IsIdentifierStart(stream.Peek(1).GetUnicodeValue())) {
        return ScanResult::NotHandled;
    }

    const std::size_t start = stream.Position();
    const source::SourceLocation location = stream.Peek().GetLocation();

    scanner_utils::AdvanceAndRecord(context);
    const std::size_t end = scanner_utils::ScanIdentifierEnd(context);

    const auto span = scanner_utils::MakeSpan(context, start, end);
    const std::string_view lexeme = scanner_utils::SliceLexeme(context, start, end);

    context.PushToken(TokenFactory::Make(TokenType::Annotation, lexeme, location, span));
    return ScanResult::TokenProduced;
}

}  // namespace myc::lexer
