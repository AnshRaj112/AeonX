#include "myc/parser/future_pattern_parser.hpp"

#include "myc/lexer/token_type.hpp"

namespace myc::parser {

FuturePatternParser::FuturePatternParser(ParserContext& context) : context_(context) {}

bool FuturePatternParser::IsPatternStart() const {
    const lexer::TokenType type = context_.Peek().GetType();
    return type == lexer::TokenType::Identifier || type == lexer::TokenType::IntegerLiteral ||
           type == lexer::TokenType::KwTrue || type == lexer::TokenType::KwFalse ||
           type == lexer::TokenType::DelimLeftParen || type == lexer::TokenType::DelimLeftBracket;
}

}  // namespace myc::parser
