#pragma once

#include "myc/lexer/token.hpp"

#include <vector>

namespace myc::lexer {

/// Factory for constructing immutable tokens. The lexer is the primary caller.
class TokenFactory {
public:
    [[nodiscard]] static Token Make(TokenType type, std::string_view lexeme,
                                    source::SourceLocation location, source::SourceSpan span,
                                    TokenFlags flags = TokenFlags::None,
                                    LiteralKind literal_kind = LiteralKind::None);

    [[nodiscard]] static Token MakeIdentifier(std::string_view lexeme,
                                              source::SourceLocation location,
                                              source::SourceSpan span,
                                              TokenFlags flags = TokenFlags::None);

    [[nodiscard]] static Token MakeKeyword(TokenType type, std::string_view lexeme,
                                           source::SourceLocation location,
                                           source::SourceSpan span,
                                           TokenFlags flags = TokenFlags::None);

    [[nodiscard]] static Token MakeLiteral(TokenType type, LiteralKind literal_kind,
                                           std::string_view lexeme,
                                           source::SourceLocation location,
                                           source::SourceSpan span,
                                           TokenFlags flags = TokenFlags::None);

    [[nodiscard]] static Token MakeOperator(TokenType type, std::string_view lexeme,
                                            source::SourceLocation location,
                                            source::SourceSpan span,
                                            TokenFlags flags = TokenFlags::None);

    [[nodiscard]] static Token MakePunctuation(TokenType type, std::string_view lexeme,
                                               source::SourceLocation location,
                                               source::SourceSpan span,
                                               TokenFlags flags = TokenFlags::None);

    [[nodiscard]] static Token MakeDelimiter(TokenType type, std::string_view lexeme,
                                             source::SourceLocation location,
                                             source::SourceSpan span,
                                             TokenFlags flags = TokenFlags::None);

    [[nodiscard]] static Token MakeEOF(source::SourceLocation location, source::SourceSpan span);

    [[nodiscard]] static Token MakeError(std::string_view lexeme, source::SourceLocation location,
                                         source::SourceSpan span,
                                         TokenFlags flags = TokenFlags::Invalid);

    [[nodiscard]] static Token MakeUnknown(std::string_view lexeme,
                                           source::SourceLocation location,
                                           source::SourceSpan span,
                                           TokenFlags flags = TokenFlags::None);

    [[nodiscard]] static std::vector<Token> MakeBatch(
        std::initializer_list<Token> tokens);

private:
    [[nodiscard]] static Token Build(TokenType type, std::string_view lexeme,
                                     source::SourceLocation location, source::SourceSpan span,
                                     TokenFlags flags, LiteralKind literal_kind);
};

}  // namespace myc::lexer
