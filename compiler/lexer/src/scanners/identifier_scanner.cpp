#include "myc/lexer/scanners/identifier_scanner.hpp"

#include "myc/lexer/scanners/scanner_utils.hpp"
#include "myc/lexer/token_factory.hpp"

namespace myc::lexer {

ScanResult IdentifierScanner::Scan(LexerContext& context) {
    CharacterStream& stream = context.GetStream();
    if (stream.Eof()) {
        return ScanResult::NotHandled;
    }

    if (!scanner_utils::IsIdentifierStart(stream.Peek().GetUnicodeValue())) {
        return ScanResult::NotHandled;
    }

    const std::size_t start = stream.Position();
    const source::SourceLocation location = stream.Peek().GetLocation();
    const std::size_t end = scanner_utils::ScanIdentifierEnd(context);

    const std::string_view lexeme = scanner_utils::SliceLexeme(context, start, end);
    const auto span = scanner_utils::MakeSpan(context, start, end);

    context.PushToken(TokenFactory::MakeIdentifier(lexeme, location, span));
    return ScanResult::TokenProduced;
}

}  // namespace myc::lexer
