#pragma once

#include "myc/lexer/scanners/scanner.hpp"

namespace myc::lexer {

class CharacterScanner : public IScanner {
public:
    [[nodiscard]] ScanResult Scan(LexerContext&) override { return ScanResult::NotHandled; }
    [[nodiscard]] const char* GetName() const noexcept override { return "CharacterScanner"; }
};

}  // namespace myc::lexer
