#pragma once

#include "myc/lexer/scanners/scanner.hpp"

#include <memory>
#include <span>
#include <vector>

namespace myc::lexer {

/// Ordered collection of scanners tried during each lexing step.
///
/// Dispatch order matters: trivia and comments are tried before literals and
/// identifiers so leading whitespace does not interfere with token boundaries.
class ScannerRegistry {
public:
    ScannerRegistry();

    [[nodiscard]] std::span<const std::unique_ptr<IScanner>> GetScanners() const noexcept {
        return scanners_;
    }

    [[nodiscard]] ScanResult Dispatch(LexerContext& context);

private:
    void RegisterScanner(std::unique_ptr<IScanner> scanner);

    std::vector<std::unique_ptr<IScanner>> scanners_;
};

}  // namespace myc::lexer
