#pragma once

#include "myc/lexer/scanners/scanner.hpp"

namespace myc::lexer {

class KeywordScanner : public IScanner {
public:
    [[nodiscard]] ScanResult Scan(LexerContext&) override { return ScanResult::NotHandled; }
    [[nodiscard]] const char* GetName() const noexcept override { return "KeywordScanner"; }
};

}  // namespace myc::lexer
