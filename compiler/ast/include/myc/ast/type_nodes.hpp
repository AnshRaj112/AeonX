#pragma once

#include "myc/ast/ast_node.hpp"
#include "myc/ast/identifier_node.hpp"
#include "myc/lexer/token_type.hpp"

#include <memory>
#include <string>
#include <vector>

namespace myc::ast {

class PrimitiveTypeNode final : public TypeNode {
public:
    PrimitiveTypeNode() = default;
    explicit PrimitiveTypeNode(lexer::TokenType token_type, source::SourceSpan span = {})
        : TypeNode(span), token_type_(token_type) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::PrimitiveType; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] lexer::TokenType GetTokenType() const noexcept { return token_type_; }

private:
    lexer::TokenType token_type_{lexer::TokenType::TyInt};
};

class NamedTypeNode final : public TypeNode {
public:
    NamedTypeNode() = default;
    explicit NamedTypeNode(std::unique_ptr<IdentifierNode> name, source::SourceSpan span = {})
        : TypeNode(span), name_(std::move(name)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::NamedType; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetName() const noexcept { return name_.get(); }
    [[nodiscard]] IdentifierNode* GetName() noexcept { return name_.get(); }

private:
    std::unique_ptr<IdentifierNode> name_;
};

class FutureGenericTypeNode final : public TypeNode {
public:
    FutureGenericTypeNode() = default;
    explicit FutureGenericTypeNode(std::unique_ptr<TypeNode> base, source::SourceSpan span = {})
        : TypeNode(span), base_(std::move(base)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::FutureGenericType; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const TypeNode* GetBase() const noexcept { return base_.get(); }

private:
    std::unique_ptr<TypeNode> base_;
};

}  // namespace myc::ast
