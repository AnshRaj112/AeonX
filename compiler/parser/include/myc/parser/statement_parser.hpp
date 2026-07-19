#pragma once

#include "myc/ast/statement_nodes.hpp"
#include "myc/parser/expression_parser.hpp"
#include "myc/parser/parser_context.hpp"
#include "myc/parser/recovery_manager.hpp"
#include "myc/parser/type_parser.hpp"

#include <memory>

namespace myc::parser {

/// Parses statements and blocks.
class StatementParser {
public:
    explicit StatementParser(ParserContext& context);

    [[nodiscard]] std::unique_ptr<ast::StatementNode> ParseStatement();
    [[nodiscard]] std::unique_ptr<ast::BlockStatementNode> ParseBlock();

private:
    [[nodiscard]] std::unique_ptr<ast::StatementNode> ParseIfStatement();
    [[nodiscard]] std::unique_ptr<ast::StatementNode> ParseWhileStatement();
    [[nodiscard]] std::unique_ptr<ast::StatementNode> ParseForStatement();
    [[nodiscard]] std::unique_ptr<ast::StatementNode> ParseReturnStatement();
    [[nodiscard]] std::unique_ptr<ast::StatementNode> ParseVariableDeclaration();
    [[nodiscard]] std::unique_ptr<ast::StatementNode> ParseExpressionStatement();
    [[nodiscard]] std::unique_ptr<ast::IdentifierNode> ParseIdentifier();

    ParserContext& context_;
    ExpressionParser expression_parser_;
    TypeParser type_parser_;
};

}  // namespace myc::parser
