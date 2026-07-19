#pragma once

#include "myc/lexer/token_type.hpp"
#include "myc/parser/parser_context.hpp"

namespace myc::parser {

/// Synchronization-based error recovery for the parser.
class RecoveryManager {
public:
    explicit RecoveryManager(ParserContext& context);

    void Synchronize();
    void SynchronizeStatement();
    void SynchronizeDeclaration();
    void SynchronizeBlock();

    [[nodiscard]] bool IsSynchronizationToken(lexer::TokenType type) const noexcept;
    [[nodiscard]] bool IsDeclarationStart(lexer::TokenType type) const noexcept;

private:
    ParserContext& context_;
};

}  // namespace myc::parser
