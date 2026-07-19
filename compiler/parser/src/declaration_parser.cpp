#include "myc/parser/declaration_parser.hpp"

#include "myc/lexer/token_type.hpp"

namespace myc::parser {

DeclarationParser::DeclarationParser(ParserContext& context)
    : context_(context),
      statement_parser_(context),
      type_parser_(context),
      attribute_parser_(context),
      import_parser_(context) {}

std::vector<std::unique_ptr<ast::DeclarationNode>> DeclarationParser::ParseDeclarations() {
    std::vector<std::unique_ptr<ast::DeclarationNode>> declarations;

    while (!context_.GetCursor().IsDone()) {
        if (!IsDeclarationStart()) {
            if (context_.Peek().IsEOF()) {
                break;
            }
            context_.ReportError(context_.Peek(), "expected declaration");
            context_.Recover();
            continue;
        }

        auto declaration = ParseDeclaration();
        if (declaration != nullptr) {
            declarations.push_back(std::move(declaration));
        } else {
            context_.Recover();
        }
    }

    return declarations;
}

std::unique_ptr<ast::DeclarationNode> DeclarationParser::ParseDeclaration() {
    if (context_.Peek().Is(lexer::TokenType::KwImport)) {
        return import_parser_.ParseImport();
    }

    if (context_.Peek().Is(lexer::TokenType::KwPackage)) {
        return ParsePackageDeclaration();
    }

    auto attributes = attribute_parser_.ParseAttributes();

    bool is_public = false;
    if (context_.Match(lexer::TokenType::KwPub)) {
        is_public = true;
    }

    bool is_async = false;
    if (context_.Match(lexer::TokenType::KwAsync)) {
        is_async = true;
    }

    const lexer::Token token = context_.Peek();
    std::unique_ptr<ast::DeclarationNode> declaration;

    if (token.Is(lexer::TokenType::KwFn)) {
        declaration = ParseFunctionDeclaration(is_public, is_async, std::move(attributes));
    } else if (token.Is(lexer::TokenType::KwStruct)) {
        declaration = ParseStructDeclaration(is_public);
    } else if (token.Is(lexer::TokenType::KwClass)) {
        declaration = ParseClassDeclaration(is_public);
    } else if (token.Is(lexer::TokenType::KwTrait)) {
        declaration = ParseTraitDeclaration(is_public);
    } else if (token.Is(lexer::TokenType::KwEnum)) {
        declaration = ParseEnumDeclaration(is_public);
    } else if (token.Is(lexer::TokenType::KwMod)) {
        declaration = ParseModuleDeclaration(is_public);
    } else {
        context_.ReportError(token, "expected declaration");
        return nullptr;
    }

    if (declaration != nullptr) {
        context_.RecordDeclaration();
    }

    return declaration;
}

bool DeclarationParser::IsDeclarationStart() const {
    const lexer::TokenType type = context_.Peek().GetType();
    if (type == lexer::TokenType::KwImport || type == lexer::TokenType::KwPackage ||
        type == lexer::TokenType::KwFn || type == lexer::TokenType::KwStruct ||
        type == lexer::TokenType::KwClass || type == lexer::TokenType::KwTrait ||
        type == lexer::TokenType::KwEnum || type == lexer::TokenType::KwMod ||
        type == lexer::TokenType::Annotation) {
        return true;
    }

    if (type == lexer::TokenType::KwPub || type == lexer::TokenType::KwAsync) {
        return true;
    }

    return false;
}

std::unique_ptr<ast::DeclarationNode> DeclarationParser::ParseFunctionDeclaration(
    bool is_public, bool is_async,
    std::vector<std::unique_ptr<ast::AttributeNode>> attributes) {
    const auto start = context_.Peek().GetSpan();
    (void)context_.Consume();

    auto name = ParseIdentifier();
    if (name == nullptr) {
        return nullptr;
    }

    if (!context_.Expect(lexer::TokenType::DelimLeftParen, "expected '(' after function name")) {
        context_.Recover();
    }

    auto parameters = ParseParameters();
    if (!context_.Expect(lexer::TokenType::DelimRightParen, "expected ')' after parameters")) {
        context_.Recover();
    }

    std::unique_ptr<ast::TypeNode> return_type;
    if (context_.Match(lexer::TokenType::PunctColon)) {
        return_type = type_parser_.ParseType();
    }

    auto body = statement_parser_.ParseBlock();
    source::SourceSpan span = start;
    if (body != nullptr) {
        span = ast::ExtendSpan(span, body->GetSpan());
    }

    return context_.MakeNode<ast::FunctionDeclarationNode>(
        std::move(name), is_public, is_async, std::move(attributes), std::move(parameters),
        std::move(return_type), std::move(body), span);
}

std::unique_ptr<ast::DeclarationNode> DeclarationParser::ParseStructDeclaration(bool is_public) {
    const auto start = context_.Peek().GetSpan();
    (void)context_.Consume();

    auto name = ParseIdentifier();
    if (name == nullptr) {
        return nullptr;
    }

    if (!context_.Expect(lexer::TokenType::DelimLeftBrace, "expected '{' after struct name")) {
        context_.Recover();
    }

    auto fields = ParseFields();

    source::SourceSpan span = start;
    if (!context_.Expect(lexer::TokenType::DelimRightBrace, "expected '}' after struct fields")) {
        context_.Recover();
    } else {
        span = ast::ExtendSpan(span, context_.Previous().GetSpan());
    }

    (void)context_.Match(lexer::TokenType::PunctSemicolon);

    return context_.MakeNode<ast::StructDeclarationNode>(std::move(name), is_public,
                                                         std::move(fields), span);
}

std::unique_ptr<ast::DeclarationNode> DeclarationParser::ParseClassDeclaration(bool is_public) {
    const auto start = context_.Peek().GetSpan();
    (void)context_.Consume();

    auto name = ParseIdentifier();
    if (name == nullptr) {
        return nullptr;
    }

    std::vector<std::unique_ptr<ast::DeclarationNode>> members;
    if (context_.Match(lexer::TokenType::DelimLeftBrace)) {
        while (!context_.Peek().Is(lexer::TokenType::DelimRightBrace) &&
               !context_.GetCursor().IsDone()) {
            if (IsDeclarationStart()) {
                auto member = ParseDeclaration();
                if (member != nullptr) {
                    members.push_back(std::move(member));
                }
            } else {
                context_.GetRecovery().SynchronizeDeclaration();
            }
        }
        (void)context_.Expect(lexer::TokenType::DelimRightBrace, "expected '}' after class body");
    }

    source::SourceSpan span = start;
    return context_.MakeNode<ast::ClassDeclarationNode>(std::move(name), is_public,
                                                        std::move(members), span);
}

std::unique_ptr<ast::DeclarationNode> DeclarationParser::ParseTraitDeclaration(bool is_public) {
    const auto start = context_.Peek().GetSpan();
    (void)context_.Consume();

    auto name = ParseIdentifier();
    if (name == nullptr) {
        return nullptr;
    }

    std::vector<std::unique_ptr<ast::DeclarationNode>> members;
    if (context_.Match(lexer::TokenType::DelimLeftBrace)) {
        while (!context_.Peek().Is(lexer::TokenType::DelimRightBrace) &&
               !context_.GetCursor().IsDone()) {
            if (IsDeclarationStart()) {
                auto member = ParseDeclaration();
                if (member != nullptr) {
                    members.push_back(std::move(member));
                }
            } else {
                context_.GetRecovery().SynchronizeDeclaration();
            }
        }
        (void)context_.Expect(lexer::TokenType::DelimRightBrace, "expected '}' after trait body");
    }

    source::SourceSpan span = start;
    return context_.MakeNode<ast::TraitDeclarationNode>(std::move(name), is_public,
                                                        std::move(members), span);
}

std::unique_ptr<ast::DeclarationNode> DeclarationParser::ParseEnumDeclaration(bool is_public) {
    const auto start = context_.Peek().GetSpan();
    (void)context_.Consume();

    auto name = ParseIdentifier();
    if (name == nullptr) {
        return nullptr;
    }

    std::vector<std::unique_ptr<ast::IdentifierNode>> variants;
    if (context_.Match(lexer::TokenType::DelimLeftBrace)) {
        while (!context_.Peek().Is(lexer::TokenType::DelimRightBrace) &&
               !context_.GetCursor().IsDone()) {
            auto variant = ParseIdentifier();
            if (variant != nullptr) {
                variants.push_back(std::move(variant));
            }
            if (!context_.Match(lexer::TokenType::PunctComma)) {
                break;
            }
        }
        (void)context_.Expect(lexer::TokenType::DelimRightBrace, "expected '}' after enum variants");
    }

    source::SourceSpan span = start;
    (void)context_.Match(lexer::TokenType::PunctSemicolon);
    return context_.MakeNode<ast::EnumDeclarationNode>(std::move(name), is_public,
                                                       std::move(variants), span);
}

std::unique_ptr<ast::DeclarationNode> DeclarationParser::ParseModuleDeclaration(bool is_public) {
    const auto start = context_.Peek().GetSpan();
    (void)context_.Consume();

    auto name = ParseIdentifier();
    if (name == nullptr) {
        return nullptr;
    }

    std::unique_ptr<ast::BlockStatementNode> body;
    if (context_.Peek().Is(lexer::TokenType::DelimLeftBrace)) {
        body = statement_parser_.ParseBlock();
    }

    source::SourceSpan span = start;
    if (body != nullptr) {
        span = ast::ExtendSpan(span, body->GetSpan());
    }

    return context_.MakeNode<ast::ModuleDeclarationNode>(std::move(name), is_public,
                                                         std::move(body), span);
}

std::unique_ptr<ast::DeclarationNode> DeclarationParser::ParsePackageDeclaration() {
    const auto start = context_.Peek().GetSpan();
    (void)context_.Consume();

    auto name = ParseIdentifier();
    source::SourceSpan span = start;
    if (!context_.Expect(lexer::TokenType::PunctSemicolon, "expected ';' after package")) {
        context_.Recover();
    } else {
        span = ast::ExtendSpan(span, context_.Previous().GetSpan());
    }

    context_.RecordDeclaration();
    return context_.MakeNode<ast::PackageDeclarationNode>(std::move(name), span);
}

std::unique_ptr<ast::IdentifierNode> DeclarationParser::ParseIdentifier() {
    const lexer::Token token = context_.Peek();
    if (!token.Is(lexer::TokenType::Identifier)) {
        context_.ReportError(token, "expected identifier");
        return nullptr;
    }
    const std::string name(token.GetLexeme());
    (void)context_.Consume();
    return context_.MakeNode<ast::IdentifierNode>(name, token.GetSpan());
}

std::vector<std::unique_ptr<ast::ParameterNode>> DeclarationParser::ParseParameters() {
    std::vector<std::unique_ptr<ast::ParameterNode>> parameters;

    if (context_.Peek().Is(lexer::TokenType::DelimRightParen)) {
        return parameters;
    }

    do {
        auto param_name = ParseIdentifier();
        if (param_name == nullptr) {
            break;
        }

        if (!context_.Expect(lexer::TokenType::PunctColon, "expected ':' after parameter name")) {
            context_.Recover();
            break;
        }

        auto param_type = type_parser_.ParseType();
        auto span = ast::ExtendSpan(param_name->GetSpan(),
                                    param_type != nullptr ? param_type->GetSpan()
                                                          : param_name->GetSpan());
        parameters.push_back(
            context_.MakeNode<ast::ParameterNode>(std::move(param_name), std::move(param_type), span));
    } while (context_.Match(lexer::TokenType::PunctComma));

    return parameters;
}

std::vector<std::unique_ptr<ast::FieldNode>> DeclarationParser::ParseFields() {
    std::vector<std::unique_ptr<ast::FieldNode>> fields;

    while (!context_.Peek().Is(lexer::TokenType::DelimRightBrace) &&
           !context_.GetCursor().IsDone()) {
        auto field_name = ParseIdentifier();
        if (field_name == nullptr) {
            context_.GetRecovery().SynchronizeDeclaration();
            continue;
        }

        if (!context_.Expect(lexer::TokenType::PunctColon, "expected ':' after field name")) {
            context_.Recover();
            continue;
        }

        auto field_type = type_parser_.ParseType();
        auto span = ast::ExtendSpan(field_name->GetSpan(),
                                    field_type != nullptr ? field_type->GetSpan()
                                                          : field_name->GetSpan());
        fields.push_back(
            context_.MakeNode<ast::FieldNode>(std::move(field_name), std::move(field_type), span));

        if (!context_.Match(lexer::TokenType::PunctComma)) {
            break;
        }
    }

    return fields;
}

}  // namespace myc::parser
