#include "myc/parser/recovery_manager.hpp"

#include "myc/lexer/token_type.hpp"

namespace myc::parser {

RecoveryManager::RecoveryManager(ParserContext& context) : context_(context) {}

bool RecoveryManager::IsSynchronizationToken(lexer::TokenType type) const noexcept {
    return type == lexer::TokenType::PunctSemicolon ||
           type == lexer::TokenType::DelimRightBrace ||
           type == lexer::TokenType::EndOfFile || IsDeclarationStart(type);
}

bool RecoveryManager::IsDeclarationStart(lexer::TokenType type) const noexcept {
    return type == lexer::TokenType::KwFn || type == lexer::TokenType::KwClass ||
           type == lexer::TokenType::KwStruct || type == lexer::TokenType::KwTrait ||
           type == lexer::TokenType::KwEnum || type == lexer::TokenType::KwImport ||
           type == lexer::TokenType::KwMod || type == lexer::TokenType::KwPackage ||
           type == lexer::TokenType::KwPub || type == lexer::TokenType::KwAsync ||
           type == lexer::TokenType::Annotation;
}

void RecoveryManager::Synchronize() {
    if (context_.GetCursor().IsDone()) {
        return;
    }

    context_.GetState().in_panic_mode = true;
    while (!context_.GetCursor().IsDone()) {
        const lexer::TokenType type = context_.Peek().GetType();
        if (IsSynchronizationToken(type)) {
            if (type == lexer::TokenType::PunctSemicolon ||
                type == lexer::TokenType::DelimRightBrace) {
                (void)context_.Consume();
            }
            return;
        }
        (void)context_.Consume();
    }
}

void RecoveryManager::SynchronizeStatement() {
    while (!context_.GetCursor().IsDone()) {
        const lexer::TokenType type = context_.Peek().GetType();
        if (type == lexer::TokenType::PunctSemicolon || type == lexer::TokenType::DelimRightBrace ||
            type == lexer::TokenType::EndOfFile) {
            if (type == lexer::TokenType::PunctSemicolon) {
                (void)context_.Consume();
            }
            return;
        }
        (void)context_.Consume();
    }
}

void RecoveryManager::SynchronizeDeclaration() {
    while (!context_.GetCursor().IsDone()) {
        const lexer::TokenType type = context_.Peek().GetType();
        if (type == lexer::TokenType::DelimRightBrace || type == lexer::TokenType::EndOfFile ||
            IsDeclarationStart(type)) {
            return;
        }
        (void)context_.Consume();
    }
}

void RecoveryManager::SynchronizeBlock() {
    int depth = 1;
    while (!context_.GetCursor().IsDone() && depth > 0) {
        const lexer::TokenType type = context_.Peek().GetType();
        if (type == lexer::TokenType::DelimLeftBrace) {
            ++depth;
        } else if (type == lexer::TokenType::DelimRightBrace) {
            --depth;
        }
        (void)context_.Consume();
    }
}

}  // namespace myc::parser
