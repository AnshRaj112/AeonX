#include "myc/lexer/scanners/unknown_scanner.hpp"

#include "myc/lexer/scanners/scanner_utils.hpp"

namespace myc::lexer {

ScanResult UnknownScanner::Scan(LexerContext& context) {
    CharacterStream& stream = context.GetStream();
    if (stream.Eof()) {
        return ScanResult::NotHandled;
    }

    const std::size_t start = stream.Position();
    scanner_utils::AdvanceAndRecord(context);
    const std::size_t end = stream.Position();

    scanner_utils::PushErrorToken(context, start, end, "unexpected character in lexical analysis");
    context.MarkError();
    return ScanResult::TokenProduced;
}

}  // namespace myc::lexer
