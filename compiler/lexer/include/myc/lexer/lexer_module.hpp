#pragma once

namespace myc::lexer {

/// Placeholder interface for the lexer subsystem.
class LexerModule {
public:
    [[nodiscard]] static const char* Name() noexcept;
};

}  // namespace myc::lexer
