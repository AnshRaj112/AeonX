#include "myc/parser/token_cursor.hpp"

namespace myc::parser {

TokenCursor::TokenCursor(lexer::TokenStream stream) : stream_(std::move(stream)) {}

const lexer::Token& TokenCursor::Peek(std::size_t offset) const { return stream_.Peek(offset); }

const lexer::Token& TokenCursor::LookAhead(std::size_t count) const {
    return stream_.Lookahead(count);
}

const lexer::Token& TokenCursor::Consume() { return stream_.Consume(); }

const lexer::Token& TokenCursor::Previous() const { return stream_.Previous(); }

bool TokenCursor::Match(lexer::TokenType type) {
    if (stream_.IsAtEnd() || !stream_.Peek().Is(type)) {
        return false;
    }
    (void)stream_.Consume();
    return true;
}

bool TokenCursor::MatchOneOf(std::initializer_list<lexer::TokenType> types) {
    if (stream_.IsAtEnd()) {
        return false;
    }
    if (!stream_.Peek().IsOneOf(types)) {
        return false;
    }
    (void)stream_.Consume();
    return true;
}

bool TokenCursor::Expect(lexer::TokenType type) {
    if (stream_.IsAtEnd() || !stream_.Peek().Is(type)) {
        return false;
    }
    (void)stream_.Consume();
    return true;
}

std::size_t TokenCursor::Checkpoint() const noexcept { return stream_.SaveCheckpoint(); }

void TokenCursor::Rewind(std::size_t checkpoint) noexcept {
    stream_.RestoreCheckpoint(checkpoint);
}

bool TokenCursor::IsAtEnd() const noexcept { return stream_.IsAtEnd(); }

bool TokenCursor::IsDone() const noexcept {
    return stream_.IsAtEnd() || stream_.Peek().IsEOF();
}

std::size_t TokenCursor::Position() const noexcept { return stream_.GetPosition(); }

}  // namespace myc::parser
