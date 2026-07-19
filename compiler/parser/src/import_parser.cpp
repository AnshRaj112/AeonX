#include "myc/parser/import_parser.hpp"

#include "myc/lexer/token_type.hpp"

namespace myc::parser {

ImportParser::ImportParser(ParserContext& context) : context_(context) {}

std::unique_ptr<ast::ImportDeclarationNode> ImportParser::ParseImport() {
    const auto start = context_.Peek().GetSpan();
    if (!context_.Expect(lexer::TokenType::KwImport, "expected 'import'")) {
        return nullptr;
    }

    auto path = ParsePath();
    source::SourceSpan span = start;
    if (!path.empty()) {
        span = ast::ExtendSpan(span, path.back()->GetSpan());
    }

    if (!context_.Expect(lexer::TokenType::PunctSemicolon, "expected ';' after import")) {
        context_.Recover();
    } else {
        span = ast::ExtendSpan(span, context_.Previous().GetSpan());
    }

    context_.RecordDeclaration();
    return context_.MakeNode<ast::ImportDeclarationNode>(std::move(path), span);
}

std::vector<std::unique_ptr<ast::IdentifierNode>> ImportParser::ParsePath() {
    std::vector<std::unique_ptr<ast::IdentifierNode>> segments;

    do {
        const lexer::Token token = context_.Peek();
        if (!token.Is(lexer::TokenType::Identifier)) {
            context_.ReportError(token, "expected identifier in import path");
            break;
        }
        const std::string name(token.GetLexeme());
        (void)context_.Consume();
        segments.push_back(context_.MakeNode<ast::IdentifierNode>(name, token.GetSpan()));
    } while (context_.Match(lexer::TokenType::OpColonColon));

    return segments;
}

}  // namespace myc::parser
