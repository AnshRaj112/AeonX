#pragma once

#include "myc/ast/ast_node.hpp"
#include "myc/lexer/token_type.hpp"

#include <string>
#include <string_view>

namespace myc::ast {

class IntegerLiteralNode final : public LiteralNode {
public:
    IntegerLiteralNode() = default;
    IntegerLiteralNode(std::string value, source::SourceSpan span = {})
        : LiteralNode(span), value_(std::move(value)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::IntegerLiteral; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] std::string_view GetValue() const noexcept { return value_; }

private:
    std::string value_;
};

class FloatLiteralNode final : public LiteralNode {
public:
    FloatLiteralNode() = default;
    FloatLiteralNode(std::string value, source::SourceSpan span = {})
        : LiteralNode(span), value_(std::move(value)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::FloatLiteral; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] std::string_view GetValue() const noexcept { return value_; }

private:
    std::string value_;
};

class BooleanLiteralNode final : public LiteralNode {
public:
    BooleanLiteralNode() = default;
    explicit BooleanLiteralNode(bool value, source::SourceSpan span = {})
        : LiteralNode(span), value_(value) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::BooleanLiteral; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] bool GetValue() const noexcept { return value_; }

private:
    bool value_{false};
};

class StringLiteralNode final : public LiteralNode {
public:
    StringLiteralNode() = default;
    StringLiteralNode(std::string value, source::SourceSpan span = {})
        : LiteralNode(span), value_(std::move(value)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::StringLiteral; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] std::string_view GetValue() const noexcept { return value_; }

private:
    std::string value_;
};

class CharacterLiteralNode final : public LiteralNode {
public:
    CharacterLiteralNode() = default;
    CharacterLiteralNode(std::string value, source::SourceSpan span = {})
        : LiteralNode(span), value_(std::move(value)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::CharacterLiteral; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] std::string_view GetValue() const noexcept { return value_; }

private:
    std::string value_;
};

}  // namespace myc::ast
