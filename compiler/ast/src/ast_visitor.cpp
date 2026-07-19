#include "myc/ast/ast_visitor.hpp"

namespace myc::ast {

#define MYC_VISITOR_DEFAULT(NodeType, VisitMethod) \
    void AstVisitor::VisitMethod(NodeType&) {}     \
    void AstConstVisitor::VisitMethod(const NodeType&) {}

MYC_VISITOR_DEFAULT(ProgramNode, VisitProgram)
MYC_VISITOR_DEFAULT(FunctionDeclarationNode, VisitFunctionDeclaration)
MYC_VISITOR_DEFAULT(ClassDeclarationNode, VisitClassDeclaration)
MYC_VISITOR_DEFAULT(StructDeclarationNode, VisitStructDeclaration)
MYC_VISITOR_DEFAULT(TraitDeclarationNode, VisitTraitDeclaration)
MYC_VISITOR_DEFAULT(EnumDeclarationNode, VisitEnumDeclaration)
MYC_VISITOR_DEFAULT(ImportDeclarationNode, VisitImportDeclaration)
MYC_VISITOR_DEFAULT(ModuleDeclarationNode, VisitModuleDeclaration)
MYC_VISITOR_DEFAULT(PackageDeclarationNode, VisitPackageDeclaration)
MYC_VISITOR_DEFAULT(ExpressionStatementNode, VisitExpressionStatement)
MYC_VISITOR_DEFAULT(VariableDeclarationNode, VisitVariableDeclaration)
MYC_VISITOR_DEFAULT(ReturnStatementNode, VisitReturnStatement)
MYC_VISITOR_DEFAULT(IfStatementNode, VisitIfStatement)
MYC_VISITOR_DEFAULT(WhileStatementNode, VisitWhileStatement)
MYC_VISITOR_DEFAULT(ForStatementNode, VisitForStatement)
MYC_VISITOR_DEFAULT(BlockStatementNode, VisitBlockStatement)
MYC_VISITOR_DEFAULT(FutureSwitchStatementNode, VisitFutureSwitchStatement)
MYC_VISITOR_DEFAULT(FutureAsyncStatementNode, VisitFutureAsyncStatement)
MYC_VISITOR_DEFAULT(IdentifierExpressionNode, VisitIdentifierExpression)
MYC_VISITOR_DEFAULT(LiteralExpressionNode, VisitLiteralExpression)
MYC_VISITOR_DEFAULT(CallExpressionNode, VisitCallExpression)
MYC_VISITOR_DEFAULT(MemberAccessExpressionNode, VisitMemberAccessExpression)
MYC_VISITOR_DEFAULT(UnaryExpressionNode, VisitUnaryExpression)
MYC_VISITOR_DEFAULT(BinaryExpressionNode, VisitBinaryExpression)
MYC_VISITOR_DEFAULT(ParenthesizedExpressionNode, VisitParenthesizedExpression)
MYC_VISITOR_DEFAULT(AssignmentExpressionNode, VisitAssignmentExpression)
MYC_VISITOR_DEFAULT(FutureLambdaExpressionNode, VisitFutureLambdaExpression)
MYC_VISITOR_DEFAULT(FutureMatchExpressionNode, VisitFutureMatchExpression)
MYC_VISITOR_DEFAULT(PrimitiveTypeNode, VisitPrimitiveType)
MYC_VISITOR_DEFAULT(NamedTypeNode, VisitNamedType)
MYC_VISITOR_DEFAULT(FutureGenericTypeNode, VisitFutureGenericType)
MYC_VISITOR_DEFAULT(IntegerLiteralNode, VisitIntegerLiteral)
MYC_VISITOR_DEFAULT(FloatLiteralNode, VisitFloatLiteral)
MYC_VISITOR_DEFAULT(BooleanLiteralNode, VisitBooleanLiteral)
MYC_VISITOR_DEFAULT(StringLiteralNode, VisitStringLiteral)
MYC_VISITOR_DEFAULT(CharacterLiteralNode, VisitCharacterLiteral)
MYC_VISITOR_DEFAULT(IdentifierNode, VisitIdentifier)
MYC_VISITOR_DEFAULT(ParameterNode, VisitParameter)
MYC_VISITOR_DEFAULT(FieldNode, VisitField)
MYC_VISITOR_DEFAULT(AttributeNode, VisitAttribute)
MYC_VISITOR_DEFAULT(FutureModuleNode, VisitFutureModule)
MYC_VISITOR_DEFAULT(FutureTraitNode, VisitFutureTrait)
MYC_VISITOR_DEFAULT(FutureGenericNode, VisitFutureGeneric)

#undef MYC_VISITOR_DEFAULT

void AstVisitor::VisitNode(AstNode& node) { node.Accept(*this); }
void AstConstVisitor::VisitNode(const AstNode& node) { node.Accept(*this); }

}  // namespace myc::ast
