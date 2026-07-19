#include "myc/ast/ast_visitor.hpp"
#include "myc/ast/declaration_nodes.hpp"
#include "myc/ast/expression_nodes.hpp"
#include "myc/ast/future_nodes.hpp"
#include "myc/ast/identifier_node.hpp"
#include "myc/ast/literal_nodes.hpp"
#include "myc/ast/program_node.hpp"
#include "myc/ast/statement_nodes.hpp"
#include "myc/ast/type_nodes.hpp"

namespace myc::ast {

#define MYC_AST_ACCEPT(NodeType, VisitMethod)              \
    void NodeType::Accept(AstVisitor& visitor) {           \
        visitor.VisitMethod(*this);                        \
    }                                                      \
    void NodeType::Accept(AstConstVisitor& visitor) const { \
        visitor.VisitMethod(*this);                        \
    }

MYC_AST_ACCEPT(ProgramNode, VisitProgram)
MYC_AST_ACCEPT(FunctionDeclarationNode, VisitFunctionDeclaration)
MYC_AST_ACCEPT(ClassDeclarationNode, VisitClassDeclaration)
MYC_AST_ACCEPT(StructDeclarationNode, VisitStructDeclaration)
MYC_AST_ACCEPT(TraitDeclarationNode, VisitTraitDeclaration)
MYC_AST_ACCEPT(EnumDeclarationNode, VisitEnumDeclaration)
MYC_AST_ACCEPT(ImportDeclarationNode, VisitImportDeclaration)
MYC_AST_ACCEPT(ModuleDeclarationNode, VisitModuleDeclaration)
MYC_AST_ACCEPT(PackageDeclarationNode, VisitPackageDeclaration)
MYC_AST_ACCEPT(ExpressionStatementNode, VisitExpressionStatement)
MYC_AST_ACCEPT(VariableDeclarationNode, VisitVariableDeclaration)
MYC_AST_ACCEPT(ReturnStatementNode, VisitReturnStatement)
MYC_AST_ACCEPT(IfStatementNode, VisitIfStatement)
MYC_AST_ACCEPT(WhileStatementNode, VisitWhileStatement)
MYC_AST_ACCEPT(ForStatementNode, VisitForStatement)
MYC_AST_ACCEPT(BlockStatementNode, VisitBlockStatement)
MYC_AST_ACCEPT(FutureSwitchStatementNode, VisitFutureSwitchStatement)
MYC_AST_ACCEPT(FutureAsyncStatementNode, VisitFutureAsyncStatement)
MYC_AST_ACCEPT(IdentifierExpressionNode, VisitIdentifierExpression)
MYC_AST_ACCEPT(LiteralExpressionNode, VisitLiteralExpression)
MYC_AST_ACCEPT(CallExpressionNode, VisitCallExpression)
MYC_AST_ACCEPT(MemberAccessExpressionNode, VisitMemberAccessExpression)
MYC_AST_ACCEPT(UnaryExpressionNode, VisitUnaryExpression)
MYC_AST_ACCEPT(BinaryExpressionNode, VisitBinaryExpression)
MYC_AST_ACCEPT(ParenthesizedExpressionNode, VisitParenthesizedExpression)
MYC_AST_ACCEPT(AssignmentExpressionNode, VisitAssignmentExpression)
MYC_AST_ACCEPT(FutureLambdaExpressionNode, VisitFutureLambdaExpression)
MYC_AST_ACCEPT(FutureMatchExpressionNode, VisitFutureMatchExpression)
MYC_AST_ACCEPT(PrimitiveTypeNode, VisitPrimitiveType)
MYC_AST_ACCEPT(NamedTypeNode, VisitNamedType)
MYC_AST_ACCEPT(FutureGenericTypeNode, VisitFutureGenericType)
MYC_AST_ACCEPT(IntegerLiteralNode, VisitIntegerLiteral)
MYC_AST_ACCEPT(FloatLiteralNode, VisitFloatLiteral)
MYC_AST_ACCEPT(BooleanLiteralNode, VisitBooleanLiteral)
MYC_AST_ACCEPT(StringLiteralNode, VisitStringLiteral)
MYC_AST_ACCEPT(CharacterLiteralNode, VisitCharacterLiteral)
MYC_AST_ACCEPT(IdentifierNode, VisitIdentifier)
MYC_AST_ACCEPT(ParameterNode, VisitParameter)
MYC_AST_ACCEPT(FieldNode, VisitField)
MYC_AST_ACCEPT(AttributeNode, VisitAttribute)
MYC_AST_ACCEPT(FutureModuleNode, VisitFutureModule)
MYC_AST_ACCEPT(FutureTraitNode, VisitFutureTrait)
MYC_AST_ACCEPT(FutureGenericNode, VisitFutureGeneric)

#undef MYC_AST_ACCEPT

}  // namespace myc::ast
