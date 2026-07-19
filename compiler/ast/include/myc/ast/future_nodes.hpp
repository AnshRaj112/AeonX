#pragma once

#include "myc/ast/ast_node.hpp"
#include "myc/ast/identifier_node.hpp"

#include <memory>

namespace myc::ast {

/// Placeholder for future module system extensions.
class FutureModuleNode final : public AstNode {
public:
    FutureModuleNode() = default;
    explicit FutureModuleNode(std::unique_ptr<IdentifierNode> name, source::SourceSpan span = {})
        : AstNode(span), name_(std::move(name)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::FutureModule; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetName() const noexcept { return name_.get(); }

private:
    std::unique_ptr<IdentifierNode> name_;
};

/// Placeholder for future trait system extensions.
class FutureTraitNode final : public AstNode {
public:
    FutureTraitNode() = default;
    explicit FutureTraitNode(std::unique_ptr<IdentifierNode> name, source::SourceSpan span = {})
        : AstNode(span), name_(std::move(name)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::FutureTrait; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetName() const noexcept { return name_.get(); }

private:
    std::unique_ptr<IdentifierNode> name_;
};

/// Placeholder for future generic system extensions.
class FutureGenericNode final : public AstNode {
public:
    FutureGenericNode() = default;
    explicit FutureGenericNode(std::unique_ptr<IdentifierNode> name, source::SourceSpan span = {})
        : AstNode(span), name_(std::move(name)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::FutureGeneric; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetName() const noexcept { return name_.get(); }

private:
    std::unique_ptr<IdentifierNode> name_;
};

}  // namespace myc::ast
