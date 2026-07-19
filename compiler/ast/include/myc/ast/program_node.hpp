#pragma once

#include "myc/ast/ast_node.hpp"
#include "myc/ast/declaration_nodes.hpp"

#include <memory>
#include <vector>

namespace myc::ast {

class ProgramNode final : public AstNode {
public:
    ProgramNode() = default;
    explicit ProgramNode(std::vector<std::unique_ptr<DeclarationNode>> declarations,
                        source::SourceSpan span = {})
        : AstNode(span), declarations_(std::move(declarations)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::Program; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const std::vector<std::unique_ptr<DeclarationNode>>& GetDeclarations() const
        noexcept {
        return declarations_;
    }

private:
    std::vector<std::unique_ptr<DeclarationNode>> declarations_;
};

}  // namespace myc::ast
