#include "myc/lexer/scanners/keyword_scanner.hpp"

#include "myc/lexer/keyword.hpp"
#include "myc/lexer/scanners/scanner_utils.hpp"
#include "myc/lexer/token_factory.hpp"

namespace myc::lexer {

ScanResult KeywordScanner::Scan(LexerContext& context) {
    CharacterStream& stream = context.GetStream();
    if (stream.Eof()) {
        return ScanResult::NotHandled;
    }

    if (!scanner_utils::IsIdentifierStart(stream.Peek().GetUnicodeValue())) {
        return ScanResult::NotHandled;
    }

    const std::size_t start = stream.Position();
    const source::SourceLocation location = stream.Peek().GetLocation();
    const std::size_t end = scanner_utils::MeasureIdentifierEnd(stream);

    const std::string_view text = scanner_utils::SliceLexeme(context, start, end);
    const Keyword* keyword = KeywordTable::Lookup(text);
    if (keyword == nullptr || (!IsKeyword(keyword->type) && !IsPrimitiveType(keyword->type))) {
        return ScanResult::NotHandled;
    }

    scanner_utils::ConsumeTo(context, end);
    const auto span = scanner_utils::MakeSpan(context, start, end);
    context.PushToken(TokenFactory::MakeKeyword(keyword->type, text, location, span));
    return ScanResult::TokenProduced;
}

}  // namespace myc::lexer
