#pragma once

#include "myc/ast/ast_node.hpp"
#include "myc/ast/identifier_node.hpp"
#include "myc/ast/statement_nodes.hpp"
#include "myc/ast/type_nodes.hpp"

#include <memory>
#include <string>
#include <vector>

namespace myc::ast {

class AttributeNode final : public AstNode {
public:
    AttributeNode() = default;
    explicit AttributeNode(std::string name, source::SourceSpan span = {})
        : AstNode(span), name_(std::move(name)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::Attribute; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] std::string_view GetName() const noexcept { return name_; }

private:
    std::string name_;
};

class ParameterNode final : public AstNode {
public:
    ParameterNode() = default;
    ParameterNode(std::unique_ptr<IdentifierNode> name, std::unique_ptr<TypeNode> type,
                 source::SourceSpan span = {})
        : AstNode(span), name_(std::move(name)), type_(std::move(type)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::Parameter; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetName() const noexcept { return name_.get(); }
    [[nodiscard]] const TypeNode* GetType() const noexcept { return type_.get(); }

private:
    std::unique_ptr<IdentifierNode> name_;
    std::unique_ptr<TypeNode> type_;
};

class FieldNode final : public AstNode {
public:
    FieldNode() = default;
    FieldNode(std::unique_ptr<IdentifierNode> name, std::unique_ptr<TypeNode> type,
             source::SourceSpan span = {})
        : AstNode(span), name_(std::move(name)), type_(std::move(type)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::Field; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetName() const noexcept { return name_.get(); }
    [[nodiscard]] const TypeNode* GetType() const noexcept { return type_.get(); }

private:
    std::unique_ptr<IdentifierNode> name_;
    std::unique_ptr<TypeNode> type_;
};

class FunctionDeclarationNode final : public DeclarationNode {
public:
    FunctionDeclarationNode() = default;
    FunctionDeclarationNode(std::unique_ptr<IdentifierNode> name, bool is_public, bool is_async,
                           std::vector<std::unique_ptr<AttributeNode>> attributes,
                           std::vector<std::unique_ptr<ParameterNode>> parameters,
                           std::unique_ptr<TypeNode> return_type,
                           std::unique_ptr<BlockStatementNode> body, source::SourceSpan span = {})
        : DeclarationNode(span),
          name_(std::move(name)),
          is_public_(is_public),
          is_async_(is_async),
          attributes_(std::move(attributes)),
          parameters_(std::move(parameters)),
          return_type_(std::move(return_type)),
          body_(std::move(body)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::FunctionDeclaration; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetName() const noexcept { return name_.get(); }
    [[nodiscard]] bool IsPublic() const noexcept { return is_public_; }
    [[nodiscard]] bool IsAsync() const noexcept { return is_async_; }
    [[nodiscard]] const std::vector<std::unique_ptr<AttributeNode>>& GetAttributes() const noexcept {
        return attributes_;
    }
    [[nodiscard]] const std::vector<std::unique_ptr<ParameterNode>>& GetParameters() const noexcept {
        return parameters_;
    }
    [[nodiscard]] const TypeNode* GetReturnType() const noexcept { return return_type_.get(); }
    [[nodiscard]] const BlockStatementNode* GetBody() const noexcept { return body_.get(); }

private:
    std::unique_ptr<IdentifierNode> name_;
    bool is_public_{false};
    bool is_async_{false};
    std::vector<std::unique_ptr<AttributeNode>> attributes_;
    std::vector<std::unique_ptr<ParameterNode>> parameters_;
    std::unique_ptr<TypeNode> return_type_;
    std::unique_ptr<BlockStatementNode> body_;
};

class ClassDeclarationNode final : public DeclarationNode {
public:
    ClassDeclarationNode() = default;
    ClassDeclarationNode(std::unique_ptr<IdentifierNode> name, bool is_public,
                        std::vector<std::unique_ptr<DeclarationNode>> members,
                        source::SourceSpan span = {})
        : DeclarationNode(span),
          name_(std::move(name)),
          is_public_(is_public),
          members_(std::move(members)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::ClassDeclaration; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetName() const noexcept { return name_.get(); }
    [[nodiscard]] bool IsPublic() const noexcept { return is_public_; }
    [[nodiscard]] const std::vector<std::unique_ptr<DeclarationNode>>& GetMembers() const noexcept {
        return members_;
    }

private:
    std::unique_ptr<IdentifierNode> name_;
    bool is_public_{false};
    std::vector<std::unique_ptr<DeclarationNode>> members_;
};

class StructDeclarationNode final : public DeclarationNode {
public:
    StructDeclarationNode() = default;
    StructDeclarationNode(std::unique_ptr<IdentifierNode> name, bool is_public,
                         std::vector<std::unique_ptr<FieldNode>> fields,
                         source::SourceSpan span = {})
        : DeclarationNode(span),
          name_(std::move(name)),
          is_public_(is_public),
          fields_(std::move(fields)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::StructDeclaration; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetName() const noexcept { return name_.get(); }
    [[nodiscard]] bool IsPublic() const noexcept { return is_public_; }
    [[nodiscard]] const std::vector<std::unique_ptr<FieldNode>>& GetFields() const noexcept {
        return fields_;
    }

private:
    std::unique_ptr<IdentifierNode> name_;
    bool is_public_{false};
    std::vector<std::unique_ptr<FieldNode>> fields_;
};

class TraitDeclarationNode final : public DeclarationNode {
public:
    TraitDeclarationNode() = default;
    TraitDeclarationNode(std::unique_ptr<IdentifierNode> name, bool is_public,
                        std::vector<std::unique_ptr<DeclarationNode>> members,
                        source::SourceSpan span = {})
        : DeclarationNode(span),
          name_(std::move(name)),
          is_public_(is_public),
          members_(std::move(members)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::TraitDeclaration; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetName() const noexcept { return name_.get(); }
    [[nodiscard]] bool IsPublic() const noexcept { return is_public_; }
    [[nodiscard]] const std::vector<std::unique_ptr<DeclarationNode>>& GetMembers() const noexcept {
        return members_;
    }

private:
    std::unique_ptr<IdentifierNode> name_;
    bool is_public_{false};
    std::vector<std::unique_ptr<DeclarationNode>> members_;
};

class EnumDeclarationNode final : public DeclarationNode {
public:
    EnumDeclarationNode() = default;
    EnumDeclarationNode(std::unique_ptr<IdentifierNode> name, bool is_public,
                       std::vector<std::unique_ptr<IdentifierNode>> variants,
                       source::SourceSpan span = {})
        : DeclarationNode(span),
          name_(std::move(name)),
          is_public_(is_public),
          variants_(std::move(variants)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::EnumDeclaration; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetName() const noexcept { return name_.get(); }
    [[nodiscard]] bool IsPublic() const noexcept { return is_public_; }
    [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& GetVariants() const noexcept {
        return variants_;
    }

private:
    std::unique_ptr<IdentifierNode> name_;
    bool is_public_{false};
    std::vector<std::unique_ptr<IdentifierNode>> variants_;
};

class ImportDeclarationNode final : public DeclarationNode {
public:
    ImportDeclarationNode() = default;
    ImportDeclarationNode(std::vector<std::unique_ptr<IdentifierNode>> path_segments,
                          source::SourceSpan span = {})
        : DeclarationNode(span), path_segments_(std::move(path_segments)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::ImportDeclaration; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& GetPathSegments() const
        noexcept {
        return path_segments_;
    }

private:
    std::vector<std::unique_ptr<IdentifierNode>> path_segments_;
};

class ModuleDeclarationNode final : public DeclarationNode {
public:
    ModuleDeclarationNode() = default;
    ModuleDeclarationNode(std::unique_ptr<IdentifierNode> name, bool is_public,
                           std::unique_ptr<BlockStatementNode> body, source::SourceSpan span = {})
        : DeclarationNode(span),
          name_(std::move(name)),
          is_public_(is_public),
          body_(std::move(body)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::ModuleDeclaration; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetName() const noexcept { return name_.get(); }
    [[nodiscard]] bool IsPublic() const noexcept { return is_public_; }
    [[nodiscard]] const BlockStatementNode* GetBody() const noexcept { return body_.get(); }

private:
    std::unique_ptr<IdentifierNode> name_;
    bool is_public_{false};
    std::unique_ptr<BlockStatementNode> body_;
};

class PackageDeclarationNode final : public DeclarationNode {
public:
    PackageDeclarationNode() = default;
    explicit PackageDeclarationNode(std::unique_ptr<IdentifierNode> name, source::SourceSpan span = {})
        : DeclarationNode(span), name_(std::move(name)) {}

    [[nodiscard]] AstKind GetKind() const noexcept override { return AstKind::PackageDeclaration; }
    void Accept(AstVisitor& visitor) override;
    void Accept(AstConstVisitor& visitor) const override;

    [[nodiscard]] const IdentifierNode* GetName() const noexcept { return name_.get(); }

private:
    std::unique_ptr<IdentifierNode> name_;
};

}  // namespace myc::ast
