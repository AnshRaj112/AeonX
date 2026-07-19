#pragma once

#include "myc/lexer/token.hpp"
#include "myc/lexer/token_stream.hpp"
#include "myc/lexer/token_type.hpp"

#include <cstddef>
#include <initializer_list>

namespace myc::parser {

/// Thin wrapper around TokenStream providing parser-oriented navigation.
class TokenCursor {
public:
    explicit TokenCursor(lexer::TokenStream stream);

    [[nodiscard]] const lexer::Token& Peek(std::size_t offset = 0) const;
    [[nodiscard]] const lexer::Token& LookAhead(std::size_t count) const;
    [[nodiscard]] const lexer::Token& Consume();
    [[nodiscard]] const lexer::Token& Previous() const;

    [[nodiscard]] bool Match(lexer::TokenType type);
    [[nodiscard]] bool MatchOneOf(std::initializer_list<lexer::TokenType> types);
    [[nodiscard]] bool Expect(lexer::TokenType type);

    [[nodiscard]] std::size_t Checkpoint() const noexcept;
    void Rewind(std::size_t checkpoint) noexcept;

    [[nodiscard]] bool IsAtEnd() const noexcept;
    [[nodiscard]] bool IsDone() const noexcept;
    [[nodiscard]] std::size_t Position() const noexcept;
    [[nodiscard]] lexer::TokenStream& GetStream() noexcept { return stream_; }
    [[nodiscard]] const lexer::TokenStream& GetStream() const noexcept { return stream_; }

private:
    lexer::TokenStream stream_;
};

}  // namespace myc::parser
