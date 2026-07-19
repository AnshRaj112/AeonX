#pragma once

#include <cstdint>

namespace myc::ast {

/// Discriminant for every AST node type. Grouped by category with reserved gaps.
enum class AstKind : std::uint16_t {
    // --- Root (0–9) ---
    Program = 0,

    // --- Declarations (10–49) ---
    FunctionDeclaration = 10,
    ClassDeclaration,
    StructDeclaration,
    TraitDeclaration,
    EnumDeclaration,
    ImportDeclaration,
    ModuleDeclaration,
    PackageDeclaration,

    // --- Statements (50–89) ---
    ExpressionStatement = 50,
    VariableDeclaration,
    ReturnStatement,
    IfStatement,
    WhileStatement,
    ForStatement,
    BlockStatement,
    FutureSwitchStatement,
    FutureAsyncStatement,

    // --- Expressions (90–149) ---
    IdentifierExpression = 90,
    LiteralExpression,
    CallExpression,
    MemberAccessExpression,
    UnaryExpression,
    BinaryExpression,
    ParenthesizedExpression,
    AssignmentExpression,
    FutureLambdaExpression,
    FutureMatchExpression,

    // --- Types (150–169) ---
    PrimitiveType = 150,
    NamedType,
    FutureGenericType,

    // --- Literals (170–189) ---
    IntegerLiteral = 170,
    FloatLiteral,
    BooleanLiteral,
    StringLiteral,
    CharacterLiteral,

    // --- Other (190–209) ---
    Identifier = 190,
    Parameter,
    Field,
    Attribute,

    // --- Future placeholders (210–229) ---
    FutureModule = 210,
    FutureTrait,
    FutureGeneric,
};

[[nodiscard]] constexpr bool IsDeclaration(AstKind kind) noexcept {
    const auto value = static_cast<std::uint16_t>(kind);
    return value >= static_cast<std::uint16_t>(AstKind::FunctionDeclaration) &&
           value <= static_cast<std::uint16_t>(AstKind::PackageDeclaration);
}

[[nodiscard]] constexpr bool IsStatement(AstKind kind) noexcept {
    const auto value = static_cast<std::uint16_t>(kind);
    return value >= static_cast<std::uint16_t>(AstKind::ExpressionStatement) &&
           value <= static_cast<std::uint16_t>(AstKind::FutureAsyncStatement);
}

[[nodiscard]] constexpr bool IsExpression(AstKind kind) noexcept {
    const auto value = static_cast<std::uint16_t>(kind);
    return value >= static_cast<std::uint16_t>(AstKind::IdentifierExpression) &&
           value <= static_cast<std::uint16_t>(AstKind::FutureMatchExpression);
}

}  // namespace myc::ast
