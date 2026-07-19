#include "myc/ast/ast_pretty_printer.hpp"

#include "myc/ast/declaration_nodes.hpp"
#include "myc/ast/expression_nodes.hpp"
#include "myc/ast/future_nodes.hpp"
#include "myc/ast/identifier_node.hpp"
#include "myc/ast/literal_nodes.hpp"
#include "myc/ast/program_node.hpp"
#include "myc/ast/statement_nodes.hpp"
#include "myc/ast/type_nodes.hpp"
#include "myc/lexer/token_type.hpp"

#include <sstream>

namespace myc::ast {

AstPrettyPrinter::AstPrettyPrinter(std::ostream& output, int indent_width)
    : indent_width_(indent_width) {
    (void)output;
}

std::string AstPrettyPrinter::Print(const AstNode& root) {
    buffer_.clear();
    depth_ = 0;
    root.Accept(*this);
    return buffer_;
}

void AstPrettyPrinter::WriteLine(std::string_view text) {
    buffer_.append(static_cast<std::size_t>(depth_ * indent_width_), ' ');
    buffer_.append(text);
    buffer_.push_back('\n');
}

void AstPrettyPrinter::WriteIndented(std::string_view label) { WriteLine(label); }

void AstPrettyPrinter::Indent() { ++depth_; }
void AstPrettyPrinter::Dedent() { --depth_; }

void AstPrettyPrinter::VisitExpression(const ExpressionNode* expression) {
    if (expression != nullptr) {
        expression->Accept(*this);
    }
}

void AstPrettyPrinter::VisitStatement(const StatementNode* statement) {
    if (statement != nullptr) {
        statement->Accept(*this);
    }
}

void AstPrettyPrinter::VisitDeclaration(const DeclarationNode* declaration) {
    if (declaration != nullptr) {
        declaration->Accept(*this);
    }
}

void AstPrettyPrinter::VisitType(const TypeNode* type) {
    if (type != nullptr) {
        type->Accept(*this);
    }
}

void AstPrettyPrinter::VisitLiteral(const LiteralNode* literal) {
    if (literal != nullptr) {
        literal->Accept(*this);
    }
}

void AstPrettyPrinter::VisitProgram(const ProgramNode& node) {
    WriteLine("Program");
    Indent();
    for (const auto& declaration : node.GetDeclarations()) {
        VisitDeclaration(declaration.get());
    }
    Dedent();
}

void AstPrettyPrinter::VisitFunctionDeclaration(const FunctionDeclarationNode& node) {
    WriteLine("FunctionDeclaration");
    Indent();
    if (node.IsPublic()) {
        WriteLine("Visibility: pub");
    }
    if (node.IsAsync()) {
        WriteLine("Async: true");
    }
    if (const IdentifierNode* name = node.GetName()) {
        WriteLine("Name: " + std::string(name->GetName()));
    }
    for (const auto& attribute : node.GetAttributes()) {
        if (attribute) {
            attribute->Accept(*this);
        }
    }
    WriteLine("Parameters");
    Indent();
    for (const auto& parameter : node.GetParameters()) {
        if (parameter) {
            parameter->Accept(*this);
        }
    }
    Dedent();
    WriteLine("ReturnType");
    Indent();
    VisitType(node.GetReturnType());
    Dedent();
    WriteLine("Body");
    Indent();
    if (const BlockStatementNode* body = node.GetBody()) {
        body->Accept(*this);
    }
    Dedent();
    Dedent();
}

void AstPrettyPrinter::VisitClassDeclaration(const ClassDeclarationNode& node) {
    WriteLine("ClassDeclaration");
    Indent();
    if (node.IsPublic()) {
        WriteLine("Visibility: pub");
    }
    if (const IdentifierNode* name = node.GetName()) {
        WriteLine("Name: " + std::string(name->GetName()));
    }
    for (const auto& member : node.GetMembers()) {
        VisitDeclaration(member.get());
    }
    Dedent();
}

void AstPrettyPrinter::VisitStructDeclaration(const StructDeclarationNode& node) {
    WriteLine("StructDeclaration");
    Indent();
    if (node.IsPublic()) {
        WriteLine("Visibility: pub");
    }
    if (const IdentifierNode* name = node.GetName()) {
        WriteLine("Name: " + std::string(name->GetName()));
    }
    for (const auto& field : node.GetFields()) {
        if (field) {
            field->Accept(*this);
        }
    }
    Dedent();
}

void AstPrettyPrinter::VisitTraitDeclaration(const TraitDeclarationNode& node) {
    WriteLine("TraitDeclaration");
    Indent();
    if (const IdentifierNode* name = node.GetName()) {
        WriteLine("Name: " + std::string(name->GetName()));
    }
    for (const auto& member : node.GetMembers()) {
        VisitDeclaration(member.get());
    }
    Dedent();
}

void AstPrettyPrinter::VisitEnumDeclaration(const EnumDeclarationNode& node) {
    WriteLine("EnumDeclaration");
    Indent();
    if (const IdentifierNode* name = node.GetName()) {
        WriteLine("Name: " + std::string(name->GetName()));
    }
    for (const auto& variant : node.GetVariants()) {
        if (variant) {
            WriteLine("Variant: " + std::string(variant->GetName()));
        }
    }
    Dedent();
}

void AstPrettyPrinter::VisitImportDeclaration(const ImportDeclarationNode& node) {
    WriteLine("ImportDeclaration");
    Indent();
    std::string path;
    for (const auto& segment : node.GetPathSegments()) {
        if (!path.empty()) {
            path += "::";
        }
        if (segment) {
            path += segment->GetName();
        }
    }
    WriteLine("Path: " + path);
    Dedent();
}

void AstPrettyPrinter::VisitModuleDeclaration(const ModuleDeclarationNode& node) {
    WriteLine("ModuleDeclaration");
    Indent();
    if (const IdentifierNode* name = node.GetName()) {
        WriteLine("Name: " + std::string(name->GetName()));
    }
    if (const BlockStatementNode* body = node.GetBody()) {
        body->Accept(*this);
    }
    Dedent();
}

void AstPrettyPrinter::VisitPackageDeclaration(const PackageDeclarationNode& node) {
    WriteLine("PackageDeclaration");
    Indent();
    if (const IdentifierNode* name = node.GetName()) {
        WriteLine("Name: " + std::string(name->GetName()));
    }
    Dedent();
}

void AstPrettyPrinter::VisitExpressionStatement(const ExpressionStatementNode& node) {
    WriteLine("ExpressionStatement");
    Indent();
    VisitExpression(node.GetExpression());
    Dedent();
}

void AstPrettyPrinter::VisitVariableDeclaration(const VariableDeclarationNode& node) {
    WriteLine("VariableDeclaration");
    Indent();
    if (node.IsMutable()) {
        WriteLine("Mutable: true");
    }
    if (const IdentifierNode* name = node.GetName()) {
        WriteLine("Name: " + std::string(name->GetName()));
    }
    if (node.GetType() != nullptr) {
        WriteLine("Type");
        Indent();
        VisitType(node.GetType());
        Dedent();
    }
    if (node.GetInitializer() != nullptr) {
        WriteLine("Initializer");
        Indent();
        VisitExpression(node.GetInitializer());
        Dedent();
    }
    Dedent();
}

void AstPrettyPrinter::VisitReturnStatement(const ReturnStatementNode& node) {
    WriteLine("ReturnStatement");
    Indent();
    VisitExpression(node.GetValue());
    Dedent();
}

void AstPrettyPrinter::VisitIfStatement(const IfStatementNode& node) {
    WriteLine("IfStatement");
    Indent();
    WriteLine("Condition");
    Indent();
    VisitExpression(node.GetCondition());
    Dedent();
    WriteLine("Then");
    Indent();
    VisitStatement(node.GetThenBranch());
    Dedent();
    if (node.GetElseBranch() != nullptr) {
        WriteLine("Else");
        Indent();
        VisitStatement(node.GetElseBranch());
        Dedent();
    }
    Dedent();
}

void AstPrettyPrinter::VisitWhileStatement(const WhileStatementNode& node) {
    WriteLine("WhileStatement");
    Indent();
    WriteLine("Condition");
    Indent();
    VisitExpression(node.GetCondition());
    Dedent();
    WriteLine("Body");
    Indent();
    VisitStatement(node.GetBody());
    Dedent();
    Dedent();
}

void AstPrettyPrinter::VisitForStatement(const ForStatementNode& node) {
    WriteLine("ForStatement");
    Indent();
    if (const IdentifierNode* iterator = node.GetIterator()) {
        WriteLine("Iterator: " + std::string(iterator->GetName()));
    }
    WriteLine("Iterable");
    Indent();
    VisitExpression(node.GetIterable());
    Dedent();
    WriteLine("Body");
    Indent();
    VisitStatement(node.GetBody());
    Dedent();
    Dedent();
}

void AstPrettyPrinter::VisitBlockStatement(const BlockStatementNode& node) {
    WriteLine("BlockStatement");
    Indent();
    for (const auto& statement : node.GetStatements()) {
        VisitStatement(statement.get());
    }
    Dedent();
}

void AstPrettyPrinter::VisitFutureSwitchStatement(const FutureSwitchStatementNode& node) {
    WriteLine("FutureSwitchStatement");
    Indent();
    VisitExpression(node.GetScrutinee());
    Dedent();
}

void AstPrettyPrinter::VisitFutureAsyncStatement(const FutureAsyncStatementNode& node) {
    WriteLine("FutureAsyncStatement");
    Indent();
    VisitStatement(node.GetBody());
    Dedent();
}

void AstPrettyPrinter::VisitIdentifierExpression(const IdentifierExpressionNode& node) {
    WriteLine("IdentifierExpression");
    Indent();
    if (const IdentifierNode* identifier = node.GetIdentifier()) {
        WriteLine(identifier->GetName());
    }
    Dedent();
}

void AstPrettyPrinter::VisitLiteralExpression(const LiteralExpressionNode& node) {
    WriteLine("LiteralExpression");
    Indent();
    VisitLiteral(node.GetLiteral());
    Dedent();
}

void AstPrettyPrinter::VisitCallExpression(const CallExpressionNode& node) {
    WriteLine("CallExpression");
    Indent();
    WriteLine("Callee");
    Indent();
    VisitExpression(node.GetCallee());
    Dedent();
    WriteLine("Arguments");
    Indent();
    for (const auto& argument : node.GetArguments()) {
        VisitExpression(argument.get());
    }
    Dedent();
    Dedent();
}

void AstPrettyPrinter::VisitMemberAccessExpression(const MemberAccessExpressionNode& node) {
    WriteLine("MemberAccessExpression");
    Indent();
    WriteLine("Receiver");
    Indent();
    VisitExpression(node.GetReceiver());
    Dedent();
    WriteLine("Member");
    Indent();
    if (const IdentifierNode* member = node.GetMember()) {
        WriteLine(member->GetName());
    }
    Dedent();
    Dedent();
}

void AstPrettyPrinter::VisitUnaryExpression(const UnaryExpressionNode& node) {
    WriteLine("UnaryExpression");
    Indent();
    WriteLine(std::string("Operator: ") + std::string(lexer::ToString(node.GetOperatorType())));
    WriteLine(std::string("Postfix: ") + (node.IsPostfix() ? "true" : "false"));
    WriteLine("Operand");
    Indent();
    VisitExpression(node.GetOperand());
    Dedent();
    Dedent();
}

void AstPrettyPrinter::VisitBinaryExpression(const BinaryExpressionNode& node) {
    WriteLine("BinaryExpression");
    Indent();
    WriteLine(std::string("Operator: ") + std::string(lexer::ToString(node.GetOperatorType())));
    WriteLine("Left");
    Indent();
    VisitExpression(node.GetLeft());
    Dedent();
    WriteLine("Right");
    Indent();
    VisitExpression(node.GetRight());
    Dedent();
    Dedent();
}

void AstPrettyPrinter::VisitParenthesizedExpression(const ParenthesizedExpressionNode& node) {
    WriteLine("ParenthesizedExpression");
    Indent();
    VisitExpression(node.GetExpression());
    Dedent();
}

void AstPrettyPrinter::VisitAssignmentExpression(const AssignmentExpressionNode& node) {
    WriteLine("AssignmentExpression");
    Indent();
    WriteLine(std::string("Operator: ") + std::string(lexer::ToString(node.GetOperatorType())));
    WriteLine("Target");
    Indent();
    VisitExpression(node.GetTarget());
    Dedent();
    WriteLine("Value");
    Indent();
    VisitExpression(node.GetValue());
    Dedent();
    Dedent();
}

void AstPrettyPrinter::VisitFutureLambdaExpression(const FutureLambdaExpressionNode&) {
    WriteLine("FutureLambdaExpression");
}

void AstPrettyPrinter::VisitFutureMatchExpression(const FutureMatchExpressionNode& node) {
    WriteLine("FutureMatchExpression");
    Indent();
    VisitExpression(node.GetScrutinee());
    Dedent();
}

void AstPrettyPrinter::VisitPrimitiveType(const PrimitiveTypeNode& node) {
    WriteLine(std::string("PrimitiveType(") + std::string(lexer::ToString(node.GetTokenType())) +
              ")");
}

void AstPrettyPrinter::VisitNamedType(const NamedTypeNode& node) {
    WriteLine("NamedType");
    Indent();
    if (const IdentifierNode* name = node.GetName()) {
        name->Accept(*this);
    }
    Dedent();
}

void AstPrettyPrinter::VisitFutureGenericType(const FutureGenericTypeNode& node) {
    WriteLine("FutureGenericType");
    Indent();
    VisitType(node.GetBase());
    Dedent();
}

void AstPrettyPrinter::VisitIntegerLiteral(const IntegerLiteralNode& node) {
    WriteLine("IntegerLiteral(" + std::string(node.GetValue()) + ")");
}

void AstPrettyPrinter::VisitFloatLiteral(const FloatLiteralNode& node) {
    WriteLine("FloatLiteral(" + std::string(node.GetValue()) + ")");
}

void AstPrettyPrinter::VisitBooleanLiteral(const BooleanLiteralNode& node) {
    WriteLine(std::string("BooleanLiteral(") + (node.GetValue() ? "true" : "false") + ")");
}

void AstPrettyPrinter::VisitStringLiteral(const StringLiteralNode& node) {
    WriteLine("StringLiteral(\"" + std::string(node.GetValue()) + "\")");
}

void AstPrettyPrinter::VisitCharacterLiteral(const CharacterLiteralNode& node) {
    WriteLine("CharacterLiteral('" + std::string(node.GetValue()) + "')");
}

void AstPrettyPrinter::VisitIdentifier(const IdentifierNode& node) {
    WriteLine("Identifier(" + std::string(node.GetName()) + ")");
}

void AstPrettyPrinter::VisitParameter(const ParameterNode& node) {
    WriteLine("Parameter");
    Indent();
    if (const IdentifierNode* name = node.GetName()) {
        name->Accept(*this);
    }
    VisitType(node.GetType());
    Dedent();
}

void AstPrettyPrinter::VisitField(const FieldNode& node) {
    WriteLine("Field");
    Indent();
    if (const IdentifierNode* name = node.GetName()) {
        name->Accept(*this);
    }
    VisitType(node.GetType());
    Dedent();
}

void AstPrettyPrinter::VisitAttribute(const AttributeNode& node) {
    WriteLine("Attribute(" + std::string(node.GetName()) + ")");
}

void AstPrettyPrinter::VisitFutureModule(const FutureModuleNode& node) {
    WriteLine("FutureModule");
    Indent();
    if (const IdentifierNode* name = node.GetName()) {
        name->Accept(*this);
    }
    Dedent();
}

void AstPrettyPrinter::VisitFutureTrait(const FutureTraitNode& node) {
    WriteLine("FutureTrait");
    Indent();
    if (const IdentifierNode* name = node.GetName()) {
        name->Accept(*this);
    }
    Dedent();
}

void AstPrettyPrinter::VisitFutureGeneric(const FutureGenericNode& node) {
    WriteLine("FutureGeneric");
    Indent();
    if (const IdentifierNode* name = node.GetName()) {
        name->Accept(*this);
    }
    Dedent();
}

void AstPrettyPrinter::VisitNode(const AstNode& node) { node.Accept(*this); }

}  // namespace myc::ast
