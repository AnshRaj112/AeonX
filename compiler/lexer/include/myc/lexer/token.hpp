#pragma once

#include "myc/lexer/future_macro_token.hpp"
#include "myc/lexer/future_trivia.hpp"
#include "myc/lexer/literal_kind.hpp"
#include "myc/lexer/token_category.hpp"
#include "myc/lexer/token_flags.hpp"
#include "myc/lexer/token_type.hpp"
#include "myc/source/source_location.hpp"
#include "myc/source/source_span.hpp"

#include <cstddef>
#include <initializer_list>
#include <string_view>

namespace myc::lexer {

/// Immutable lexical token produced by the lexer.
class Token final {
public:
    [[nodiscard]] TokenType GetType() const noexcept { return type_; }
    [[nodiscard]] std::string_view GetLexeme() const noexcept { return lexeme_; }
    [[nodiscard]] const source::SourceLocation& GetLocation() const noexcept { return location_; }
    [[nodiscard]] const source::SourceSpan& GetSpan() const noexcept { return span_; }
    [[nodiscard]] std::size_t GetLength() const noexcept { return length_; }
    [[nodiscard]] TokenFlags GetFlags() const noexcept { return flags_; }
    [[nodiscard]] TokenCategory GetCategory() const noexcept { return category_; }
    [[nodiscard]] LiteralKind GetLiteralKind() const noexcept { return literal_kind_; }
    [[nodiscard]] const FutureSemanticValue& GetSemanticValue() const noexcept {
        return semantic_value_;
    }
    [[nodiscard]] const FutureMacroToken& GetMacroExpansion() const noexcept {
        return macro_expansion_;
    }
    [[nodiscard]] const FutureTrivia& GetTrivia() const noexcept { return trivia_; }

    [[nodiscard]] bool Is(TokenType type) const noexcept { return type_ == type; }

    [[nodiscard]] bool IsOneOf(std::initializer_list<TokenType> types) const noexcept {
        for (const TokenType candidate : types) {
            if (type_ == candidate) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] bool IsKeyword() const noexcept { return myc::lexer::IsKeyword(type_); }
    [[nodiscard]] bool IsPrimitiveType() const noexcept {
        return myc::lexer::IsPrimitiveType(type_);
    }
    [[nodiscard]] bool IsLiteral() const noexcept { return myc::lexer::IsLiteral(type_); }
    [[nodiscard]] bool IsOperator() const noexcept { return myc::lexer::IsOperator(type_); }
    [[nodiscard]] bool IsPunctuation() const noexcept {
        return myc::lexer::IsPunctuation(type_);
    }
    [[nodiscard]] bool IsDelimiter() const noexcept { return myc::lexer::IsDelimiter(type_); }
    [[nodiscard]] bool IsDirective() const noexcept { return myc::lexer::IsDirective(type_); }
    [[nodiscard]] bool IsTrivia() const noexcept { return myc::lexer::IsTrivia(type_); }
    [[nodiscard]] bool IsEOF() const noexcept { return IsEndOfFile(type_); }
    [[nodiscard]] bool IsErrorToken() const noexcept { return IsError(type_); }
    [[nodiscard]] bool HasFlag(TokenFlags flag) const noexcept {
        return myc::lexer::HasFlag(flags_, flag);
    }

    [[nodiscard]] bool operator==(const Token& other) const noexcept = default;

private:
    friend class TokenFactory;

    Token(TokenType type, std::string_view lexeme, source::SourceLocation location,
          source::SourceSpan span, std::size_t length, TokenFlags flags, TokenCategory category,
          LiteralKind literal_kind, FutureSemanticValue semantic_value,
          FutureMacroToken macro_expansion, FutureTrivia trivia) noexcept
        : type_(type),
          lexeme_(lexeme),
          location_(location),
          span_(span),
          length_(length),
          flags_(flags),
          category_(category),
          literal_kind_(literal_kind),
          semantic_value_(semantic_value),
          macro_expansion_(macro_expansion),
          trivia_(trivia) {}

    TokenType type_;
    std::string_view lexeme_;
    source::SourceLocation location_;
    source::SourceSpan span_;
    std::size_t length_;
    TokenFlags flags_;
    TokenCategory category_;
    LiteralKind literal_kind_;
    FutureSemanticValue semantic_value_;
    FutureMacroToken macro_expansion_;
    FutureTrivia trivia_;
};

}  // namespace myc::lexer
