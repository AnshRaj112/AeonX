#pragma once

#include "myc/ast/ast_node.hpp"
#include "myc/ast/identifier_node.hpp"
#include "myc/ast/literal_nodes.hpp"
#include "myc/lexer/token_type.hpp"

#include <memory>
#include <vector>

namespace myc::ast {

class IdentifierExpressionNode final : public ExpressionNode {
public:
    IdentifierExpressionNode() = default;
    explicit IdentifierExpressionNode(std::unique_ptr<IdentifierNode> identifier,
                                      source::SourceSpan span = {})
        : ExpressionNode(span), identifier_(std::move(identifier)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::IdentifierExpression; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetIdentifier() const noexcept { return identifier_.get(); }

private:
    std::unique_ptr<IdentifierNode> identifier_;
};

class LiteralExpressionNode final : public ExpressionNode {
public:
    LiteralExpressionNode() = default;
    explicit LiteralExpressionNode(std::unique_ptr<LiteralNode> literal, source::SourceSpan span = {})
        : ExpressionNode(span), literal_(std::move(literal)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::LiteralExpression; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const LiteralNode* GetLiteral() const noexcept { return literal_.get(); }

private:
    std::unique_ptr<LiteralNode> literal_;
};

class CallExpressionNode final : public ExpressionNode {
public:
    CallExpressionNode() = default;
    CallExpressionNode(std::unique_ptr<ExpressionNode> callee,
                       std::vector<std::unique_ptr<ExpressionNode>> arguments,
                       source::SourceSpan span = {})
        : ExpressionNode(span),
          callee_(std::move(callee)),
          arguments_(std::move(arguments)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::CallExpression; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const ExpressionNode* GetCallee() const noexcept { return callee_.get(); }
    [[nodiscard]] const std::vector<std::unique_ptr<ExpressionNode>>& GetArguments() const noexcept {
        return arguments_;
    }

private:
    std::unique_ptr<ExpressionNode> callee_;
    std::vector<std::unique_ptr<ExpressionNode>> arguments_;
};

class MemberAccessExpressionNode final : public ExpressionNode {
public:
    MemberAccessExpressionNode() = default;
    MemberAccessExpressionNode(std::unique_ptr<ExpressionNode> receiver,
                               std::unique_ptr<IdentifierNode> member,
                               source::SourceSpan span = {})
        : ExpressionNode(span), receiver_(std::move(receiver)), member_(std::move(member)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::MemberAccessExpression; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const ExpressionNode* GetReceiver() const noexcept { return receiver_.get(); }
    [[nodiscard]] const IdentifierNode* GetMember() const noexcept { return member_.get(); }

private:
    std::unique_ptr<ExpressionNode> receiver_;
    std::unique_ptr<IdentifierNode> member_;
};

class UnaryExpressionNode final : public ExpressionNode {
public:
    UnaryExpressionNode() = default;
    UnaryExpressionNode(lexer::TokenType operator_type, std::unique_ptr<ExpressionNode> operand,
                       bool is_postfix, source::SourceSpan span = {})
        : ExpressionNode(span),
          operator_type_(operator_type),
          operand_(std::move(operand)),
          is_postfix_(is_postfix) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::UnaryExpression; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] lexer::TokenType GetOperatorType() const noexcept { return operator_type_; }
    [[nodiscard]] const ExpressionNode* GetOperand() const noexcept { return operand_.get(); }
    [[nodiscard]] bool IsPostfix() const noexcept { return is_postfix_; }

private:
    lexer::TokenType operator_type_{lexer::TokenType::Unknown};
    std::unique_ptr<ExpressionNode> operand_;
    bool is_postfix_{false};
};

class BinaryExpressionNode final : public ExpressionNode {
public:
    BinaryExpressionNode() = default;
    BinaryExpressionNode(lexer::TokenType operator_type, std::unique_ptr<ExpressionNode> left,
                        std::unique_ptr<ExpressionNode> right, source::SourceSpan span = {})
        : ExpressionNode(span),
          operator_type_(operator_type),
          left_(std::move(left)),
          right_(std::move(right)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::BinaryExpression; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] lexer::TokenType GetOperatorType() const noexcept { return operator_type_; }
    [[nodiscard]] const ExpressionNode* GetLeft() const noexcept { return left_.get(); }
    [[nodiscard]] const ExpressionNode* GetRight() const noexcept { return right_.get(); }

private:
    lexer::TokenType operator_type_{lexer::TokenType::Unknown};
    std::unique_ptr<ExpressionNode> left_;
    std::unique_ptr<ExpressionNode> right_;
};

class ParenthesizedExpressionNode final : public ExpressionNode {
public:
    ParenthesizedExpressionNode() = default;
    explicit ParenthesizedExpressionNode(std::unique_ptr<ExpressionNode> expression,
                                         source::SourceSpan span = {})
        : ExpressionNode(span), expression_(std::move(expression)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override {
        return AstKind::ParenthesizedExpression;
    }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const ExpressionNode* GetExpression() const noexcept { return expression_.get(); }

private:
    std::unique_ptr<ExpressionNode> expression_;
};

class AssignmentExpressionNode final : public ExpressionNode {
public:
    AssignmentExpressionNode() = default;
    AssignmentExpressionNode(lexer::TokenType operator_type, std::unique_ptr<ExpressionNode> target,
                            std::unique_ptr<ExpressionNode> value, source::SourceSpan span = {})
        : ExpressionNode(span),
          operator_type_(operator_type),
          target_(std::move(target)),
          value_(std::move(value)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::AssignmentExpression; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] lexer::TokenType GetOperatorType() const noexcept { return operator_type_; }
    [[nodiscard]] const ExpressionNode* GetTarget() const noexcept { return target_.get(); }
    [[nodiscard]] const ExpressionNode* GetValue() const noexcept { return value_.get(); }

private:
    lexer::TokenType operator_type_{lexer::TokenType::OpAssign};
    std::unique_ptr<ExpressionNode> target_;
    std::unique_ptr<ExpressionNode> value_;
};

class FutureLambdaExpressionNode final : public ExpressionNode {
public:
    FutureLambdaExpressionNode() = default;
    explicit FutureLambdaExpressionNode(source::SourceSpan span = {}) : ExpressionNode(span) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::FutureLambdaExpression; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;
};

class FutureMatchExpressionNode final : public ExpressionNode {
public:
    FutureMatchExpressionNode() = default;
    explicit FutureMatchExpressionNode(std::unique_ptr<ExpressionNode> scrutinee,
                                       source::SourceSpan span = {})
        : ExpressionNode(span), scrutinee_(std::move(scrutinee)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::FutureMatchExpression; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const ExpressionNode* GetScrutinee() const noexcept { return scrutinee_.get(); }

private:
    std::unique_ptr<ExpressionNode> scrutinee_;
};

}  // namespace myc::ast
