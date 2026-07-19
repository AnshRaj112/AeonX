#pragma once

#include "myc/ast/expression_nodes.hpp"
#include "myc/parser/literal_parser.hpp"
#include "myc/parser/parser_context.hpp"

#include <memory>

namespace myc::parser {

/// Parses expressions using recursive descent with precedence climbing.
class ExpressionParser {
public:
    explicit ExpressionParser(ParserContext& context);

    [[nodiscard]] std::unique_ptr<ast::ExpressionNode> ParseExpression();
    [[nodiscard]] std::unique_ptr<ast::ExpressionNode> ParseAssignment();
    [[nodiscard]] std::unique_ptr<ast::ExpressionNode> ParsePrimary();

private:
    enum class Precedence : int {
        Assignment = 1,
        LogicalOr = 2,
        LogicalAnd = 3,
        Equality = 4,
        Relational = 5,
        Additive = 6,
        Multiplicative = 7,
        Unary = 8,
        Postfix = 9,
        Primary = 10,
    };

    [[nodiscard]] std::unique_ptr<ast::ExpressionNode> ParseBinary(Precedence min_precedence);
    [[nodiscard]] std::unique_ptr<ast::ExpressionNode> ParseUnary();
    [[nodiscard]] std::unique_ptr<ast::ExpressionNode> ParsePostfix(
        std::unique_ptr<ast::ExpressionNode> left);
    [[nodiscard]] Precedence GetPrecedence(lexer::TokenType type) const noexcept;
    [[nodiscard]] bool IsAssignmentOperator(lexer::TokenType type) const noexcept;
    [[nodiscard]] bool IsBinaryOperator(lexer::TokenType type) const noexcept;

    ParserContext& context_;
    LiteralParser literal_parser_;
};

}  // namespace myc::parser
