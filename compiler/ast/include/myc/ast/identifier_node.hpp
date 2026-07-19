#pragma once

#include "myc/ast/ast_node.hpp"

#include <string>
#include <string_view>

namespace myc::ast {

/// Represents a user-defined identifier in the AST.
class IdentifierNode final : public AstNode {
public:
    IdentifierNode() = default;
    explicit IdentifierNode(std::string name, source::SourceSpan span = {})
        : AstNode(span), name_(std::move(name)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::Identifier; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] std::string_view GetName() const noexcept { return name_; }
    void SetName(std::string name) { name_ = std::move(name); }

private:
    std::string name_;
};

}  // namespace myc::ast
