#include "myc/parser/statement_parser.hpp"

#include "myc/lexer/token_type.hpp"

namespace myc::parser {

StatementParser::StatementParser(ParserContext& context)
    : context_(context), expression_parser_(context), type_parser_(context) {}

std::unique_ptr<ast::StatementNode> StatementParser::ParseStatement() {
    const lexer::Token token = context_.Peek();
    std::unique_ptr<ast::StatementNode> statement;

    if (token.Is(lexer::TokenType::KwLet)) {
        statement = ParseVariableDeclaration();
    } else if (token.Is(lexer::TokenType::KwReturn)) {
        statement = ParseReturnStatement();
    } else if (token.Is(lexer::TokenType::KwIf)) {
        statement = ParseIfStatement();
    } else if (token.Is(lexer::TokenType::KwWhile)) {
        statement = ParseWhileStatement();
    } else if (token.Is(lexer::TokenType::KwFor)) {
        statement = ParseForStatement();
    } else if (token.Is(lexer::TokenType::DelimLeftBrace)) {
        statement = ParseBlock();
    } else if (token.Is(lexer::TokenType::KwMatch)) {
        auto expression = expression_parser_.ParseExpression();
        auto span = expression != nullptr ? expression->GetSpan() : token.GetSpan();
        auto expr_statement =
            context_.MakeNode<ast::ExpressionStatementNode>(std::move(expression), span);
        statement = std::move(expr_statement);
    } else {
        statement = ParseExpressionStatement();
    }

    if (statement != nullptr) {
        context_.RecordStatement();
    }
    return statement;
}

std::unique_ptr<ast::BlockStatementNode> StatementParser::ParseBlock() {
    const lexer::Token start = context_.Peek();
    if (!context_.Expect(lexer::TokenType::DelimLeftBrace, "expected '{' to start block")) {
        context_.Recover();
        return nullptr;
    }

    std::vector<std::unique_ptr<ast::StatementNode>> statements;
    while (!context_.Peek().Is(lexer::TokenType::DelimRightBrace) &&
           !context_.GetCursor().IsDone()) {
        auto statement = ParseStatement();
        if (statement != nullptr) {
            statements.push_back(std::move(statement));
        } else {
            context_.GetRecovery().SynchronizeStatement();
            if (context_.Peek().Is(lexer::TokenType::DelimRightBrace)) {
                break;
            }
        }
    }

    source::SourceSpan span = start.GetSpan();
    if (!context_.Expect(lexer::TokenType::DelimRightBrace, "expected '}' to end block")) {
        context_.Recover();
    } else {
        span = ast::ExtendSpan(span, context_.Previous().GetSpan());
    }

    return context_.MakeNode<ast::BlockStatementNode>(std::move(statements), span);
}

std::unique_ptr<ast::StatementNode> StatementParser::ParseIfStatement() {
    const auto start = context_.Peek().GetSpan();
    (void)context_.Consume();

    auto condition = expression_parser_.ParseExpression();
    auto then_branch = ParseStatement();
    std::unique_ptr<ast::StatementNode> else_branch;
    if (context_.Match(lexer::TokenType::KwElse)) {
        else_branch = ParseStatement();
    }

    auto span = start;
    if (condition != nullptr) {
        span = ast::ExtendSpan(span, condition->GetSpan());
    }
    if (then_branch != nullptr) {
        span = ast::ExtendSpan(span, then_branch->GetSpan());
    }
    if (else_branch != nullptr) {
        span = ast::ExtendSpan(span, else_branch->GetSpan());
    }

    return context_.MakeNode<ast::IfStatementNode>(std::move(condition), std::move(then_branch),
                                                   std::move(else_branch), span);
}

std::unique_ptr<ast::StatementNode> StatementParser::ParseWhileStatement() {
    const auto start = context_.Peek().GetSpan();
    (void)context_.Consume();

    auto condition = expression_parser_.ParseExpression();
    auto body = ParseStatement();
    auto span = start;
    if (condition != nullptr) {
        span = ast::ExtendSpan(span, condition->GetSpan());
    }
    if (body != nullptr) {
        span = ast::ExtendSpan(span, body->GetSpan());
    }
    return context_.MakeNode<ast::WhileStatementNode>(std::move(condition), std::move(body), span);
}

std::unique_ptr<ast::StatementNode> StatementParser::ParseForStatement() {
    const auto start = context_.Peek().GetSpan();
    (void)context_.Consume();

    auto iterator = ParseIdentifier();
    if (!context_.Expect(lexer::TokenType::KwIn, "expected 'in' in for loop")) {
        context_.Recover();
    }

    auto iterable = expression_parser_.ParseExpression();
    auto body = ParseStatement();

    auto span = body != nullptr ? ast::ExtendSpan(start, body->GetSpan()) : start;
    return context_.MakeNode<ast::ForStatementNode>(std::move(iterator), std::move(iterable),
                                                    std::move(body), span);
}

std::unique_ptr<ast::StatementNode> StatementParser::ParseReturnStatement() {
    const auto start = context_.Peek().GetSpan();
    (void)context_.Consume();

    std::unique_ptr<ast::ExpressionNode> value;
    if (!context_.Peek().Is(lexer::TokenType::PunctSemicolon)) {
        value = expression_parser_.ParseExpression();
    }

    source::SourceSpan span = start;
    if (!context_.Expect(lexer::TokenType::PunctSemicolon, "expected ';' after return")) {
        context_.Recover();
    } else {
        span = ast::ExtendSpan(span, context_.Previous().GetSpan());
    }

    return context_.MakeNode<ast::ReturnStatementNode>(std::move(value), span);
}

std::unique_ptr<ast::StatementNode> StatementParser::ParseVariableDeclaration() {
    const auto start = context_.Peek().GetSpan();
    (void)context_.Consume();

    bool is_mutable = false;
    if (context_.Match(lexer::TokenType::KwMut)) {
        is_mutable = true;
    }

    auto name = ParseIdentifier();
    if (name == nullptr) {
        return nullptr;
    }

    std::unique_ptr<ast::TypeNode> type;
    if (context_.Match(lexer::TokenType::PunctColon)) {
        type = type_parser_.ParseType();
    }

    std::unique_ptr<ast::ExpressionNode> initializer;
    if (context_.Match(lexer::TokenType::OpAssign)) {
        initializer = expression_parser_.ParseExpression();
    }

    source::SourceSpan span = start;
    if (!context_.Expect(lexer::TokenType::PunctSemicolon, "expected ';' after variable declaration")) {
        context_.Recover();
    } else {
        span = ast::ExtendSpan(span, context_.Previous().GetSpan());
    }

    return context_.MakeNode<ast::VariableDeclarationNode>(std::move(name), is_mutable,
                                                           std::move(type), std::move(initializer),
                                                           span);
}

std::unique_ptr<ast::StatementNode> StatementParser::ParseExpressionStatement() {
    auto expression = expression_parser_.ParseExpression();
    source::SourceSpan span = expression != nullptr ? expression->GetSpan() : context_.Peek().GetSpan();

    if (!context_.Expect(lexer::TokenType::PunctSemicolon, "expected ';' after expression")) {
        context_.Recover();
    } else if (expression != nullptr) {
        span = ast::ExtendSpan(span, context_.Previous().GetSpan());
    }

    return context_.MakeNode<ast::ExpressionStatementNode>(std::move(expression), span);
}

std::unique_ptr<ast::IdentifierNode> StatementParser::ParseIdentifier() {
    const lexer::Token token = context_.Peek();
    if (!token.Is(lexer::TokenType::Identifier)) {
        context_.ReportError(token, "expected identifier");
        return nullptr;
    }
    const std::string name(token.GetLexeme());
    (void)context_.Consume();
    return context_.MakeNode<ast::IdentifierNode>(name, token.GetSpan());
}

}  // namespace myc::parser
