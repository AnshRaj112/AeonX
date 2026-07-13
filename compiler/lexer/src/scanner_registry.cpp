#include "myc/lexer/scanners/scanner_registry.hpp"

#include "myc/lexer/scanners/annotation_scanner.hpp"
#include "myc/lexer/scanners/character_scanner.hpp"
#include "myc/lexer/scanners/comment_scanner.hpp"
#include "myc/lexer/scanners/directive_scanner.hpp"
#include "myc/lexer/scanners/identifier_scanner.hpp"
#include "myc/lexer/scanners/keyword_scanner.hpp"
#include "myc/lexer/scanners/number_scanner.hpp"
#include "myc/lexer/scanners/operator_scanner.hpp"
#include "myc/lexer/scanners/string_scanner.hpp"
#include "myc/lexer/scanners/unknown_scanner.hpp"
#include "myc/lexer/scanners/whitespace_scanner.hpp"

namespace myc::lexer {

ScannerRegistry::ScannerRegistry() {
    RegisterScanner(std::make_unique<WhitespaceScanner>());
    RegisterScanner(std::make_unique<CommentScanner>());
    RegisterScanner(std::make_unique<DirectiveScanner>());
    RegisterScanner(std::make_unique<AnnotationScanner>());
    RegisterScanner(std::make_unique<StringScanner>());
    RegisterScanner(std::make_unique<CharacterScanner>());
    RegisterScanner(std::make_unique<NumberScanner>());
    RegisterScanner(std::make_unique<IdentifierScanner>());
    RegisterScanner(std::make_unique<KeywordScanner>());
    RegisterScanner(std::make_unique<OperatorScanner>());
    RegisterScanner(std::make_unique<UnknownScanner>());
}

void ScannerRegistry::RegisterScanner(std::unique_ptr<IScanner> scanner) {
    scanners_.push_back(std::move(scanner));
}

ScanResult ScannerRegistry::Dispatch(LexerContext& context) {
    auto& statistics = context.GetStatistics();
    for (const auto& scanner : scanners_) {
        statistics.RecordScannerCall();
        const ScanResult result = scanner->Scan(context);
        if (result != ScanResult::NotHandled) {
            return result;
        }
    }
    return ScanResult::NotHandled;
}

}  // namespace myc::lexer
