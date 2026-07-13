#pragma once

#include "myc/lexer/scanners/scanner.hpp"

namespace myc::lexer {

class OperatorScanner : public IScanner {
public:
    [[nodiscard]] ScanResult Scan(LexerContext& context) override;
    [[nodiscard]] const char* GetName() const noexcept override { return "OperatorScanner"; }
};

}  // namespace myc::lexer
