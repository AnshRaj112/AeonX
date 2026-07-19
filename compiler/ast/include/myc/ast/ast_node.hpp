#pragma once

#include "myc/ast/ast_kind.hpp"
#include "myc/source/source_span.hpp"

#include <memory>
#include <optional>
#include <string_view>

namespace myc::ast {

class AstVisitor;
class AstConstVisitor;

/// Base class for all AST nodes. Carries source span metadata for diagnostics.
class AstNode {
public:
    virtual ~AstNode() = default;

    [[nodiscard]] virtual AstKind GetKind() const noexcept = 0;
    virtual void Accept(AstVisitor& visitor) = 0;
    virtual void Accept(AstConstVisitor& visitor) const = 0;

    [[nodiscard]] const source::SourceSpan& GetSpan() const noexcept { return span_; }
    void SetSpan(source::SourceSpan span) noexcept { span_ = span; }

protected:
    AstNode() = default;
    explicit AstNode(source::SourceSpan span) : span_(span) {}

    source::SourceSpan span_;
};

class DeclarationNode : public AstNode {
public:
    [[nodiscard]] bool IsDeclaration() const noexcept { return true; }

protected:
    DeclarationNode() = default;
    explicit DeclarationNode(source::SourceSpan span) : AstNode(span) {}
};

class StatementNode : public AstNode {
public:
    [[nodiscard]] bool IsStatement() const noexcept { return true; }

protected:
    StatementNode() = default;
    explicit StatementNode(source::SourceSpan span) : AstNode(span) {}
};

class ExpressionNode : public AstNode {
public:
    [[nodiscard]] bool IsExpression() const noexcept { return true; }

protected:
    ExpressionNode() = default;
    explicit ExpressionNode(source::SourceSpan span) : AstNode(span) {}
};

class TypeNode : public AstNode {
public:
    [[nodiscard]] bool IsType() const noexcept { return true; }

protected:
    TypeNode() = default;
    explicit TypeNode(source::SourceSpan span) : AstNode(span) {}
};

class LiteralNode : public AstNode {
public:
    [[nodiscard]] bool IsLiteral() const noexcept { return true; }

protected:
    LiteralNode() = default;
    explicit LiteralNode(source::SourceSpan span) : AstNode(span) {}
};

/// Merge two spans in the same file into a covering span.
[[nodiscard]] std::optional<source::SourceSpan> MergeSpans(const source::SourceSpan& first,
                                                           const source::SourceSpan& second) noexcept;

/// Extend a span to include another span.
[[nodiscard]] source::SourceSpan ExtendSpan(const source::SourceSpan& base,
                                            const source::SourceSpan& extra) noexcept;

}  // namespace myc::ast
