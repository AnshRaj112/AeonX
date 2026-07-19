#include "myc/parser/literal_parser.hpp"

#include "myc/lexer/token_type.hpp"

namespace myc::parser {

LiteralParser::LiteralParser(ParserContext& context) : context_(context) {}

std::unique_ptr<ast::LiteralNode> LiteralParser::ParseLiteral() {
    const lexer::Token token = context_.Peek();
    if (!token.IsLiteral()) {
        context_.ReportError(token, "expected literal");
        return nullptr;
    }

    const auto span = token.GetSpan();
    const std::string value(token.GetLexeme());
    (void)context_.Consume();

    switch (token.GetType()) {
        case lexer::TokenType::IntegerLiteral:
            return context_.MakeNode<ast::IntegerLiteralNode>(value, span);
        case lexer::TokenType::FloatLiteral:
            return context_.MakeNode<ast::FloatLiteralNode>(value, span);
        case lexer::TokenType::BooleanLiteral:
            return context_.MakeNode<ast::BooleanLiteralNode>(token.GetLexeme() == "true", span);
        case lexer::TokenType::StringLiteral:
        case lexer::TokenType::RawStringLiteral:
        case lexer::TokenType::MultilineStringLiteral:
            return context_.MakeNode<ast::StringLiteralNode>(value, span);
        case lexer::TokenType::CharacterLiteral:
            return context_.MakeNode<ast::CharacterLiteralNode>(value, span);
        default:
            context_.ReportError(token, "unsupported literal type");
            return nullptr;
    }
}

}  // namespace myc::parser
