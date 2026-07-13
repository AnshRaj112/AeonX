#pragma once

#include "myc/lexer/scanners/scanner.hpp"

namespace myc::lexer {

class StringScanner : public IScanner {
public:
    [[nodiscard]] ScanResult Scan(LexerContext& context) override;
    [[nodiscard]] const char* GetName() const noexcept override { return "StringScanner"; }
};

}  // namespace myc::lexer
