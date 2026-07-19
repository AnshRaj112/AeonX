#pragma once

#include "myc/ast/ast_node.hpp"
#include "myc/ast/expression_nodes.hpp"
#include "myc/ast/identifier_node.hpp"
#include "myc/ast/type_nodes.hpp"

#include <memory>
#include <vector>

namespace myc::ast {

class ExpressionStatementNode final : public StatementNode {
public:
    ExpressionStatementNode() = default;
    explicit ExpressionStatementNode(std::unique_ptr<ExpressionNode> expression,
                                   source::SourceSpan span = {})
        : StatementNode(span), expression_(std::move(expression)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::ExpressionStatement; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const ExpressionNode* GetExpression() const noexcept { return expression_.get(); }

private:
    std::unique_ptr<ExpressionNode> expression_;
};

class VariableDeclarationNode final : public StatementNode {
public:
    VariableDeclarationNode() = default;
    VariableDeclarationNode(std::unique_ptr<IdentifierNode> name, bool is_mutable,
                           std::unique_ptr<TypeNode> type,
                           std::unique_ptr<ExpressionNode> initializer,
                           source::SourceSpan span = {})
        : StatementNode(span),
          name_(std::move(name)),
          is_mutable_(is_mutable),
          type_(std::move(type)),
          initializer_(std::move(initializer)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::VariableDeclaration; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetName() const noexcept { return name_.get(); }
    [[nodiscard]] bool IsMutable() const noexcept { return is_mutable_; }
    [[nodiscard]] const TypeNode* GetType() const noexcept { return type_.get(); }
    [[nodiscard]] const ExpressionNode* GetInitializer() const noexcept {
        return initializer_.get();
    }

private:
    std::unique_ptr<IdentifierNode> name_;
    bool is_mutable_{false};
    std::unique_ptr<TypeNode> type_;
    std::unique_ptr<ExpressionNode> initializer_;
};

class ReturnStatementNode final : public StatementNode {
public:
    ReturnStatementNode() = default;
    explicit ReturnStatementNode(std::unique_ptr<ExpressionNode> value, source::SourceSpan span = {})
        : StatementNode(span), value_(std::move(value)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::ReturnStatement; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const ExpressionNode* GetValue() const noexcept { return value_.get(); }

private:
    std::unique_ptr<ExpressionNode> value_;
};

class IfStatementNode final : public StatementNode {
public:
    IfStatementNode() = default;
    IfStatementNode(std::unique_ptr<ExpressionNode> condition,
                   std::unique_ptr<StatementNode> then_branch,
                   std::unique_ptr<StatementNode> else_branch, source::SourceSpan span = {})
        : StatementNode(span),
          condition_(std::move(condition)),
          then_branch_(std::move(then_branch)),
          else_branch_(std::move(else_branch)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::IfStatement; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const ExpressionNode* GetCondition() const noexcept { return condition_.get(); }
    [[nodiscard]] const StatementNode* GetThenBranch() const noexcept { return then_branch_.get(); }
    [[nodiscard]] const StatementNode* GetElseBranch() const noexcept { return else_branch_.get(); }

private:
    std::unique_ptr<ExpressionNode> condition_;
    std::unique_ptr<StatementNode> then_branch_;
    std::unique_ptr<StatementNode> else_branch_;
};

class WhileStatementNode final : public StatementNode {
public:
    WhileStatementNode() = default;
    WhileStatementNode(std::unique_ptr<ExpressionNode> condition,
                      std::unique_ptr<StatementNode> body, source::SourceSpan span = {})
        : StatementNode(span), condition_(std::move(condition)), body_(std::move(body)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::WhileStatement; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const ExpressionNode* GetCondition() const noexcept { return condition_.get(); }
    [[nodiscard]] const StatementNode* GetBody() const noexcept { return body_.get(); }

private:
    std::unique_ptr<ExpressionNode> condition_;
    std::unique_ptr<StatementNode> body_;
};

class ForStatementNode final : public StatementNode {
public:
    ForStatementNode() = default;
    ForStatementNode(std::unique_ptr<IdentifierNode> iterator,
                    std::unique_ptr<ExpressionNode> iterable,
                    std::unique_ptr<StatementNode> body, source::SourceSpan span = {})
        : StatementNode(span),
          iterator_(std::move(iterator)),
          iterable_(std::move(iterable)),
          body_(std::move(body)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::ForStatement; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetIterator() const noexcept { return iterator_.get(); }
    [[nodiscard]] const ExpressionNode* GetIterable() const noexcept { return iterable_.get(); }
    [[nodiscard]] const StatementNode* GetBody() const noexcept { return body_.get(); }

private:
    std::unique_ptr<IdentifierNode> iterator_;
    std::unique_ptr<ExpressionNode> iterable_;
    std::unique_ptr<StatementNode> body_;
};

class BlockStatementNode final : public StatementNode {
public:
    BlockStatementNode() = default;
    explicit BlockStatementNode(std::vector<std::unique_ptr<StatementNode>> statements,
                                source::SourceSpan span = {})
        : StatementNode(span), statements_(std::move(statements)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::BlockStatement; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const std::vector<std::unique_ptr<StatementNode>>& GetStatements() const noexcept {
        return statements_;
    }

private:
    std::vector<std::unique_ptr<StatementNode>> statements_;
};

class FutureSwitchStatementNode final : public StatementNode {
public:
    FutureSwitchStatementNode() = default;
    explicit FutureSwitchStatementNode(std::unique_ptr<ExpressionNode> scrutinee,
                                       source::SourceSpan span = {})
        : StatementNode(span), scrutinee_(std::move(scrutinee)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::FutureSwitchStatement; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const ExpressionNode* GetScrutinee() const noexcept { return scrutinee_.get(); }

private:
    std::unique_ptr<ExpressionNode> scrutinee_;
};

class FutureAsyncStatementNode final : public StatementNode {
public:
    FutureAsyncStatementNode() = default;
    explicit FutureAsyncStatementNode(std::unique_ptr<StatementNode> body,
                                      source::SourceSpan span = {})
        : StatementNode(span), body_(std::move(body)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::FutureAsyncStatement; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const StatementNode* GetBody() const noexcept { return body_.get(); }

private:
    std::unique_ptr<StatementNode> body_;
};

}  // namespace myc::ast
