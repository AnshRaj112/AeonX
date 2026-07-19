#pragma once

#include "myc/ast/ast_node.hpp"

namespace myc::ast {

class ProgramNode;
class FunctionDeclarationNode;
class ClassDeclarationNode;
class StructDeclarationNode;
class TraitDeclarationNode;
class EnumDeclarationNode;
class ImportDeclarationNode;
class ModuleDeclarationNode;
class PackageDeclarationNode;
class ExpressionStatementNode;
class VariableDeclarationNode;
class ReturnStatementNode;
class IfStatementNode;
class WhileStatementNode;
class ForStatementNode;
class BlockStatementNode;
class FutureSwitchStatementNode;
class FutureAsyncStatementNode;
class IdentifierExpressionNode;
class LiteralExpressionNode;
class CallExpressionNode;
class MemberAccessExpressionNode;
class UnaryExpressionNode;
class BinaryExpressionNode;
class ParenthesizedExpressionNode;
class AssignmentExpressionNode;
class FutureLambdaExpressionNode;
class FutureMatchExpressionNode;
class PrimitiveTypeNode;
class NamedTypeNode;
class FutureGenericTypeNode;
class IntegerLiteralNode;
class FloatLiteralNode;
class BooleanLiteralNode;
class StringLiteralNode;
class CharacterLiteralNode;
class IdentifierNode;
class ParameterNode;
class FieldNode;
class AttributeNode;
class FutureModuleNode;
class FutureTraitNode;
class FutureGenericNode;

/// Mutable visitor base for AST traversal. Default implementations are no-ops.
class AstVisitor {
public:
    virtual ~AstVisitor() = default;

    virtual void VisitProgram(ProgramNode& node);
    virtual void VisitFunctionDeclaration(FunctionDeclarationNode& node);
    virtual void VisitClassDeclaration(ClassDeclarationNode& node);
    virtual void VisitStructDeclaration(StructDeclarationNode& node);
    virtual void VisitTraitDeclaration(TraitDeclarationNode& node);
    virtual void VisitEnumDeclaration(EnumDeclarationNode& node);
    virtual void VisitImportDeclaration(ImportDeclarationNode& node);
    virtual void VisitModuleDeclaration(ModuleDeclarationNode& node);
    virtual void VisitPackageDeclaration(PackageDeclarationNode& node);
    virtual void VisitExpressionStatement(ExpressionStatementNode& node);
    virtual void VisitVariableDeclaration(VariableDeclarationNode& node);
    virtual void VisitReturnStatement(ReturnStatementNode& node);
    virtual void VisitIfStatement(IfStatementNode& node);
    virtual void VisitWhileStatement(WhileStatementNode& node);
    virtual void VisitForStatement(ForStatementNode& node);
    virtual void VisitBlockStatement(BlockStatementNode& node);
    virtual void VisitFutureSwitchStatement(FutureSwitchStatementNode& node);
    virtual void VisitFutureAsyncStatement(FutureAsyncStatementNode& node);
    virtual void VisitIdentifierExpression(IdentifierExpressionNode& node);
    virtual void VisitLiteralExpression(LiteralExpressionNode& node);
    virtual void VisitCallExpression(CallExpressionNode& node);
    virtual void VisitMemberAccessExpression(MemberAccessExpressionNode& node);
    virtual void VisitUnaryExpression(UnaryExpressionNode& node);
    virtual void VisitBinaryExpression(BinaryExpressionNode& node);
    virtual void VisitParenthesizedExpression(ParenthesizedExpressionNode& node);
    virtual void VisitAssignmentExpression(AssignmentExpressionNode& node);
    virtual void VisitFutureLambdaExpression(FutureLambdaExpressionNode& node);
    virtual void VisitFutureMatchExpression(FutureMatchExpressionNode& node);
    virtual void VisitPrimitiveType(PrimitiveTypeNode& node);
    virtual void VisitNamedType(NamedTypeNode& node);
    virtual void VisitFutureGenericType(FutureGenericTypeNode& node);
    virtual void VisitIntegerLiteral(IntegerLiteralNode& node);
    virtual void VisitFloatLiteral(FloatLiteralNode& node);
    virtual void VisitBooleanLiteral(BooleanLiteralNode& node);
    virtual void VisitStringLiteral(StringLiteralNode& node);
    virtual void VisitCharacterLiteral(CharacterLiteralNode& node);
    virtual void VisitIdentifier(IdentifierNode& node);
    virtual void VisitParameter(ParameterNode& node);
    virtual void VisitField(FieldNode& node);
    virtual void VisitAttribute(AttributeNode& node);
    virtual void VisitFutureModule(FutureModuleNode& node);
    virtual void VisitFutureTrait(FutureTraitNode& node);
    virtual void VisitFutureGeneric(FutureGenericNode& node);

    virtual void VisitNode(AstNode& node);
};

/// Const visitor base for read-only AST traversal.
class AstConstVisitor {
public:
    virtual ~AstConstVisitor() = default;

    virtual void VisitProgram(const ProgramNode& node);
    virtual void VisitFunctionDeclaration(const FunctionDeclarationNode& node);
    virtual void VisitClassDeclaration(const ClassDeclarationNode& node);
    virtual void VisitStructDeclaration(const StructDeclarationNode& node);
    virtual void VisitTraitDeclaration(const TraitDeclarationNode& node);
    virtual void VisitEnumDeclaration(const EnumDeclarationNode& node);
    virtual void VisitImportDeclaration(const ImportDeclarationNode& node);
    virtual void VisitModuleDeclaration(const ModuleDeclarationNode& node);
    virtual void VisitPackageDeclaration(const PackageDeclarationNode& node);
    virtual void VisitExpressionStatement(const ExpressionStatementNode& node);
    virtual void VisitVariableDeclaration(const VariableDeclarationNode& node);
    virtual void VisitReturnStatement(const ReturnStatementNode& node);
    virtual void VisitIfStatement(const IfStatementNode& node);
    virtual void VisitWhileStatement(const WhileStatementNode& node);
    virtual void VisitForStatement(const ForStatementNode& node);
    virtual void VisitBlockStatement(const BlockStatementNode& node);
    virtual void VisitFutureSwitchStatement(const FutureSwitchStatementNode& node);
    virtual void VisitFutureAsyncStatement(const FutureAsyncStatementNode& node);
    virtual void VisitIdentifierExpression(const IdentifierExpressionNode& node);
    virtual void VisitLiteralExpression(const LiteralExpressionNode& node);
    virtual void VisitCallExpression(const CallExpressionNode& node);
    virtual void VisitMemberAccessExpression(const MemberAccessExpressionNode& node);
    virtual void VisitUnaryExpression(const UnaryExpressionNode& node);
    virtual void VisitBinaryExpression(const BinaryExpressionNode& node);
    virtual void VisitParenthesizedExpression(const ParenthesizedExpressionNode& node);
    virtual void VisitAssignmentExpression(const AssignmentExpressionNode& node);
    virtual void VisitFutureLambdaExpression(const FutureLambdaExpressionNode& node);
    virtual void VisitFutureMatchExpression(const FutureMatchExpressionNode& node);
    virtual void VisitPrimitiveType(const PrimitiveTypeNode& node);
    virtual void VisitNamedType(const NamedTypeNode& node);
    virtual void VisitFutureGenericType(const FutureGenericTypeNode& node);
    virtual void VisitIntegerLiteral(const IntegerLiteralNode& node);
    virtual void VisitFloatLiteral(const FloatLiteralNode& node);
    virtual void VisitBooleanLiteral(const BooleanLiteralNode& node);
    virtual void VisitStringLiteral(const StringLiteralNode& node);
    virtual void VisitCharacterLiteral(const CharacterLiteralNode& node);
    virtual void VisitIdentifier(const IdentifierNode& node);
    virtual void VisitParameter(const ParameterNode& node);
    virtual void VisitField(const FieldNode& node);
    virtual void VisitAttribute(const AttributeNode& node);
    virtual void VisitFutureModule(const FutureModuleNode& node);
    virtual void VisitFutureTrait(const FutureTraitNode& node);
    virtual void VisitFutureGeneric(const FutureGenericNode& node);

    virtual void VisitNode(const AstNode& node);
};

}  // namespace myc::ast
