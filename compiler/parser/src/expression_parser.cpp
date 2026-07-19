#include "myc/parser/expression_parser.hpp"

#include "myc/lexer/token_type.hpp"

namespace myc::parser {

ExpressionParser::ExpressionParser(ParserContext& context)
    : context_(context), literal_parser_(context) {}

std::unique_ptr<ast::ExpressionNode> ExpressionParser::ParseExpression() {
    auto expression = ParseAssignment();
    context_.RecordExpression();
    return expression;
}

std::unique_ptr<ast::ExpressionNode> ExpressionParser::ParseAssignment() {
    auto left = ParseBinary(Precedence::LogicalOr);
    if (left == nullptr) {
        return nullptr;
    }

    const lexer::Token op = context_.Peek();
    if (!IsAssignmentOperator(op.GetType())) {
        return left;
    }

    const auto operator_type = op.GetType();
    (void)context_.Consume();
    auto value = ParseAssignment();
    if (value == nullptr) {
        return left;
    }

    auto span = ast::ExtendSpan(left->GetSpan(), value->GetSpan());
    return context_.MakeNode<ast::AssignmentExpressionNode>(operator_type, std::move(left),
                                                          std::move(value), span);
}

std::unique_ptr<ast::ExpressionNode> ExpressionParser::ParseBinary(Precedence min_precedence) {
    auto left = ParseUnary();
    if (left == nullptr) {
        return nullptr;
    }

    while (true) {
        const lexer::Token op = context_.Peek();
        if (!IsBinaryOperator(op.GetType())) {
            break;
        }

        const Precedence precedence = GetPrecedence(op.GetType());
        if (static_cast<int>(precedence) < static_cast<int>(min_precedence)) {
            break;
        }

        const auto operator_type = op.GetType();
        (void)context_.Consume();
        auto right = ParseBinary(static_cast<Precedence>(static_cast<int>(precedence) + 1));
        if (right == nullptr) {
            return left;
        }

        auto span = ast::ExtendSpan(left->GetSpan(), right->GetSpan());
        left = context_.MakeNode<ast::BinaryExpressionNode>(operator_type, std::move(left),
                                                            std::move(right), span);
    }

    return left;
}

std::unique_ptr<ast::ExpressionNode> ExpressionParser::ParseUnary() {
    const lexer::Token token = context_.Peek();

    if (token.IsOneOf({lexer::TokenType::OpMinus, lexer::TokenType::OpLogicalNot,
                       lexer::TokenType::PunctBang, lexer::TokenType::OpTilde,
                       lexer::TokenType::OpPlusPlus, lexer::TokenType::OpMinusMinus})) {
        const auto operator_type =
            token.Is(lexer::TokenType::PunctBang) ? lexer::TokenType::OpLogicalNot : token.GetType();
        const auto span = token.GetSpan();
        (void)context_.Consume();
        auto operand = ParseUnary();
        if (operand == nullptr) {
            return nullptr;
        }
        auto node_span = ast::ExtendSpan(span, operand->GetSpan());
        return context_.MakeNode<ast::UnaryExpressionNode>(operator_type, std::move(operand),
                                                             false, node_span);
    }

    auto primary = ParsePrimary();
    if (primary == nullptr) {
        return nullptr;
    }

    if (context_.Peek().IsOneOf({lexer::TokenType::OpPlusPlus, lexer::TokenType::OpMinusMinus})) {
        const auto operator_type = context_.Peek().GetType();
        const auto span = context_.Peek().GetSpan();
        (void)context_.Consume();
        auto node_span = ast::ExtendSpan(primary->GetSpan(), span);
        return context_.MakeNode<ast::UnaryExpressionNode>(operator_type, std::move(primary), true,
                                                           node_span);
    }

    return primary;
}

std::unique_ptr<ast::ExpressionNode> ExpressionParser::ParsePrimary() {
    const lexer::Token token = context_.Peek();

    if (token.IsLiteral() || token.Is(lexer::TokenType::KwTrue) ||
        token.Is(lexer::TokenType::KwFalse)) {
        if (token.Is(lexer::TokenType::KwTrue) || token.Is(lexer::TokenType::KwFalse)) {
            const bool value = token.Is(lexer::TokenType::KwTrue);
            const auto span = token.GetSpan();
            (void)context_.Consume();
            auto literal = context_.MakeNode<ast::BooleanLiteralNode>(value, span);
            return context_.MakeNode<ast::LiteralExpressionNode>(std::move(literal), span);
        }
        auto literal = literal_parser_.ParseLiteral();
        if (literal == nullptr) {
            return nullptr;
        }
        return context_.MakeNode<ast::LiteralExpressionNode>(std::move(literal), literal->GetSpan());
    }

    if (token.Is(lexer::TokenType::Identifier)) {
        const auto span = token.GetSpan();
        const std::string name(token.GetLexeme());
        (void)context_.Consume();
        auto identifier = context_.MakeNode<ast::IdentifierNode>(name, span);
        auto expression =
            context_.MakeNode<ast::IdentifierExpressionNode>(std::move(identifier), span);
        return ParsePostfix(std::move(expression));
    }

    if (token.Is(lexer::TokenType::DelimLeftParen)) {
        (void)context_.Consume();
        auto expression = ParseExpression();
        if (!context_.Expect(lexer::TokenType::DelimRightParen, "expected ')' after expression")) {
            context_.Recover();
        }
        if (expression == nullptr) {
            return nullptr;
        }
        auto wrapped =
            context_.MakeNode<ast::ParenthesizedExpressionNode>(std::move(expression),
                                                                expression->GetSpan());
        return ParsePostfix(std::move(wrapped));
    }

    if (token.Is(lexer::TokenType::KwMatch)) {
        (void)context_.Consume();
        auto scrutinee = ParseExpression();
        auto span = scrutinee != nullptr ? scrutinee->GetSpan() : token.GetSpan();
        return context_.MakeNode<ast::FutureMatchExpressionNode>(std::move(scrutinee), span);
    }

    context_.ReportError(token, "expected expression");
    return nullptr;
}

std::unique_ptr<ast::ExpressionNode> ExpressionParser::ParsePostfix(
    std::unique_ptr<ast::ExpressionNode> left) {
    while (left != nullptr) {
        if (context_.Match(lexer::TokenType::DelimLeftParen)) {
            std::vector<std::unique_ptr<ast::ExpressionNode>> arguments;
            if (!context_.Peek().Is(lexer::TokenType::DelimRightParen)) {
                do {
                    auto argument = ParseExpression();
                    if (argument != nullptr) {
                        arguments.push_back(std::move(argument));
                    }
                } while (context_.Match(lexer::TokenType::PunctComma));
            }
            if (!context_.Expect(lexer::TokenType::DelimRightParen,
                                 "expected ')' after arguments")) {
                context_.Recover();
            }
            auto span = left->GetSpan();
            if (!arguments.empty()) {
                span = ast::ExtendSpan(span, arguments.back()->GetSpan());
            }
            left = context_.MakeNode<ast::CallExpressionNode>(std::move(left), std::move(arguments),
                                                              span);
            continue;
        }

        if (context_.Match(lexer::TokenType::OpDot)) {
            const lexer::Token member_token = context_.Peek();
            if (!member_token.Is(lexer::TokenType::Identifier)) {
                context_.ReportError(member_token, "expected identifier after '.'");
                break;
            }
            const std::string member_name(member_token.GetLexeme());
            const auto member_span = member_token.GetSpan();
            (void)context_.Consume();
            auto member = context_.MakeNode<ast::IdentifierNode>(member_name, member_span);
            auto span = ast::ExtendSpan(left->GetSpan(), member_span);
            left = context_.MakeNode<ast::MemberAccessExpressionNode>(std::move(left),
                                                                      std::move(member), span);
            continue;
        }

        break;
    }
    return left;
}

ExpressionParser::Precedence ExpressionParser::GetPrecedence(lexer::TokenType type) const noexcept {
    switch (type) {
        case lexer::TokenType::OpLogicalOr:
            return Precedence::LogicalOr;
        case lexer::TokenType::OpLogicalAnd:
            return Precedence::LogicalAnd;
        case lexer::TokenType::OpEqual:
        case lexer::TokenType::OpNotEqual:
            return Precedence::Equality;
        case lexer::TokenType::OpLess:
        case lexer::TokenType::OpLessEqual:
        case lexer::TokenType::OpGreater:
        case lexer::TokenType::OpGreaterEqual:
            return Precedence::Relational;
        case lexer::TokenType::OpPlus:
        case lexer::TokenType::OpMinus:
            return Precedence::Additive;
        case lexer::TokenType::OpStar:
        case lexer::TokenType::OpSlash:
        case lexer::TokenType::OpPercent:
            return Precedence::Multiplicative;
        default:
            return Precedence::Primary;
    }
}

bool ExpressionParser::IsAssignmentOperator(lexer::TokenType type) const noexcept {
    return type == lexer::TokenType::OpAssign || type == lexer::TokenType::OpPlusAssign ||
           type == lexer::TokenType::OpMinusAssign || type == lexer::TokenType::OpStarAssign ||
           type == lexer::TokenType::OpSlashAssign || type == lexer::TokenType::OpPercentAssign;
}

bool ExpressionParser::IsBinaryOperator(lexer::TokenType type) const noexcept {
    return GetPrecedence(type) != Precedence::Primary;
}

}  // namespace myc::parser
