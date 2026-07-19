#include "myc/parser/type_parser.hpp"

#include "myc/lexer/token_type.hpp"

namespace myc::parser {

TypeParser::TypeParser(ParserContext& context) : context_(context) {}

bool TypeParser::IsTypeStart(lexer::TokenType type) const noexcept {
    return lexer::IsPrimitiveType(type) || type == lexer::TokenType::Identifier;
}

std::unique_ptr<ast::TypeNode> TypeParser::ParseType() {
    const lexer::Token token = context_.Peek();
    if (!IsTypeStart(token.GetType())) {
        context_.ReportError(token, "expected type");
        return nullptr;
    }

    const auto span = token.GetSpan();
    if (lexer::IsPrimitiveType(token.GetType())) {
        const auto type = token.GetType();
        (void)context_.Consume();
        return context_.MakeNode<ast::PrimitiveTypeNode>(type, span);
    }

    const std::string name(token.GetLexeme());
    (void)context_.Consume();
    auto identifier = context_.MakeNode<ast::IdentifierNode>(name, span);
    return context_.MakeNode<ast::NamedTypeNode>(std::move(identifier), span);
}

}  // namespace myc::parser
