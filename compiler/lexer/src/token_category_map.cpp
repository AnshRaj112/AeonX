#include "myc/lexer/token_category.hpp"
#include "myc/lexer/token_category_map.hpp"

namespace myc::lexer {

TokenCategory GetCategory(TokenType type) noexcept {
    if (type == TokenType::Unknown) {
        return TokenCategory::Unknown;
    }
    if (type == TokenType::Error) {
        return TokenCategory::Error;
    }
    if (type == TokenType::EndOfFile) {
        return TokenCategory::EndOfFile;
    }
    if (type == TokenType::Identifier) {
        return TokenCategory::Identifier;
    }
    if (IsKeyword(type)) {
        return TokenCategory::Keyword;
    }
    if (IsPrimitiveType(type)) {
        return TokenCategory::PrimitiveType;
    }
    if (IsLiteral(type)) {
        return TokenCategory::Literal;
    }
    if (IsOperator(type)) {
        return TokenCategory::Operator;
    }
    if (IsPunctuation(type)) {
        return TokenCategory::Punctuation;
    }
    if (IsDelimiter(type)) {
        return TokenCategory::Delimiter;
    }
    if (type == TokenType::AttrStart) {
        return TokenCategory::Attribute;
    }
    if (type == TokenType::Annotation) {
        return TokenCategory::Annotation;
    }
    if (IsDirective(type)) {
        return TokenCategory::Directive;
    }
    if (type == TokenType::Comment) {
        return TokenCategory::Comment;
    }
    if (type == TokenType::Whitespace || type == TokenType::Newline) {
        return TokenCategory::Whitespace;
    }
    return TokenCategory::Unknown;
}

}  // namespace myc::lexer
