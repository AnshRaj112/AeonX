#include "myc/parser/attribute_parser.hpp"

#include "myc/lexer/token_type.hpp"

namespace myc::parser {

AttributeParser::AttributeParser(ParserContext& context) : context_(context) {}

std::vector<std::unique_ptr<ast::AttributeNode>> AttributeParser::ParseAttributes() {
    std::vector<std::unique_ptr<ast::AttributeNode>> attributes;

    while (context_.Peek().Is(lexer::TokenType::Annotation)) {
        const lexer::Token token = context_.Consume();
        std::string name(token.GetLexeme());
        if (!name.empty() && name.front() == '@') {
            name.erase(name.begin());
        }
        attributes.push_back(context_.MakeNode<ast::AttributeNode>(std::move(name), token.GetSpan()));
    }

    return attributes;
}

}  // namespace myc::parser
