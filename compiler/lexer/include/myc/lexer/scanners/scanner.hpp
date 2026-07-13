#pragma once

#include "myc/lexer/lexer_context.hpp"

#include <cstdint>

namespace myc::lexer {

/// Result of a single scanner invocation.
enum class ScanResult : std::uint8_t {
    /// Scanner produced one or more tokens (stored in LexerContext).
    TokenProduced,
    /// Scanner consumed input without emitting a token (e.g. whitespace).
    Consumed,
    /// Scanner did not recognize the current input.
    NotHandled,
    /// Scanner encountered a recoverable lexical error.
    Error,
};

/// Abstract scanner interface. Each lexical construct has a dedicated scanner.
class IScanner {
public:
    virtual ~IScanner() = default;

    [[nodiscard]] virtual ScanResult Scan(LexerContext& context) = 0;
    [[nodiscard]] virtual const char* GetName() const noexcept = 0;
};

}  // namespace myc::lexer
