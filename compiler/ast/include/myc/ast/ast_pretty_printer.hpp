#pragma once

#include "myc/ast/ast_visitor.hpp"

#include <ostream>
#include <string>

namespace myc::ast {

/// Debug-only AST pretty printer for inspecting parse results.
class AstPrettyPrinter final : public AstConstVisitor {
public:
    explicit AstPrettyPrinter(std::ostream& output, int indent_width = 2);

    [[nodiscard]] std::string Print(const AstNode& root);

    void VisitProgram(const ProgramNode& node) override;
    void VisitFunctionDeclaration(const FunctionDeclarationNode& node) override;
    void VisitClassDeclaration(const ClassDeclarationNode& node) override;
    void VisitStructDeclaration(const StructDeclarationNode& node) override;
    void VisitTraitDeclaration(const TraitDeclarationNode& node) override;
    void VisitEnumDeclaration(const EnumDeclarationNode& node) override;
    void VisitImportDeclaration(const ImportDeclarationNode& node) override;
    void VisitModuleDeclaration(const ModuleDeclarationNode& node) override;
    void VisitPackageDeclaration(const PackageDeclarationNode& node) override;
    void VisitExpressionStatement(const ExpressionStatementNode& node) override;
    void VisitVariableDeclaration(const VariableDeclarationNode& node) override;
    void VisitReturnStatement(const ReturnStatementNode& node) override;
    void VisitIfStatement(const IfStatementNode& node) override;
    void VisitWhileStatement(const WhileStatementNode& node) override;
    void VisitForStatement(const ForStatementNode& node) override;
    void VisitBlockStatement(const BlockStatementNode& node) override;
    void VisitFutureSwitchStatement(const FutureSwitchStatementNode& node) override;
    void VisitFutureAsyncStatement(const FutureAsyncStatementNode& node) override;
    void VisitIdentifierExpression(const IdentifierExpressionNode& node) override;
    void VisitLiteralExpression(const LiteralExpressionNode& node) override;
    void VisitCallExpression(const CallExpressionNode& node) override;
    void VisitMemberAccessExpression(const MemberAccessExpressionNode& node) override;
    void VisitUnaryExpression(const UnaryExpressionNode& node) override;
    void VisitBinaryExpression(const BinaryExpressionNode& node) override;
    void VisitParenthesizedExpression(const ParenthesizedExpressionNode& node) override;
    void VisitAssignmentExpression(const AssignmentExpressionNode& node) override;
    void VisitFutureLambdaExpression(const FutureLambdaExpressionNode& node) override;
    void VisitFutureMatchExpression(const FutureMatchExpressionNode& node) override;
    void VisitPrimitiveType(const PrimitiveTypeNode& node) override;
    void VisitNamedType(const NamedTypeNode& node) override;
    void VisitFutureGenericType(const FutureGenericTypeNode& node) override;
    void VisitIntegerLiteral(const IntegerLiteralNode& node) override;
    void VisitFloatLiteral(const FloatLiteralNode& node) override;
    void VisitBooleanLiteral(const BooleanLiteralNode& node) override;
    void VisitStringLiteral(const StringLiteralNode& node) override;
    void VisitCharacterLiteral(const CharacterLiteralNode& node) override;
    void VisitIdentifier(const IdentifierNode& node) override;
    void VisitParameter(const ParameterNode& node) override;
    void VisitField(const FieldNode& node) override;
    void VisitAttribute(const AttributeNode& node) override;
    void VisitFutureModule(const FutureModuleNode& node) override;
    void VisitFutureTrait(const FutureTraitNode& node) override;
    void VisitFutureGeneric(const FutureGenericNode& node) override;
    void VisitNode(const AstNode& node) override;

private:
    void WriteLine(std::string_view text);
    void WriteIndented(std::string_view label);
    void Indent();
    void Dedent();
    void VisitExpression(const ExpressionNode* expression);
    void VisitStatement(const StatementNode* statement);
    void VisitDeclaration(const DeclarationNode* declaration);
    void VisitType(const TypeNode* type);
    void VisitLiteral(const LiteralNode* literal);

    int indent_width_;
    int depth_{0};
    std::string buffer_;
};

}  // namespace myc::ast
