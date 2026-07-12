#pragma once

#include <cstdint>
#include <string_view>

namespace myc::lexer {

/// Comprehensive token kind enumeration. Values are grouped by category with
/// reserved gaps for future language growth.
enum class TokenType : std::uint16_t {
    // --- Special (0–9) ---
    Unknown = 0,
    Error = 1,
    EndOfFile = 2,

    // --- Identifiers (10–19) ---
    Identifier = 10,

    // --- Keywords (100–199) ---
    KwAlignof = 100,
    KwAnd,
    KwAs,
    KwAsync,
    KwAwait,
    KwBreak,
    KwCase,
    KwCatch,
    KwClass,
    KwConst,
    KwContinue,
    KwDefault,
    KwDefer,
    KwElse,
    KwEnum,
    KwExtern,
    KwFalse,
    KwFn,
    KwFor,
    KwIf,
    KwImpl,
    KwImport,
    KwIn,
    KwIs,
    KwLet,
    KwLoop,
    KwMatch,
    KwMod,
    KwMut,
    KwNil,
    KwNot,
    KwOr,
    KwOverride,
    KwPackage,
    KwPriv,
    KwPub,
    KwReturn,
    KwSelf,
    KwSelfType,
    KwSizeof,
    KwStatic,
    KwStruct,
    KwSuper,
    KwThrow,
    KwTrue,
    KwTry,
    KwTypeof,
    KwUnsafe,
    KwUse,
    KwVirtual,
    KwVoid,
    KwWhere,
    KwWhile,
    KwXor,
    KwYield,

    // --- Primitive types (200–249) ---
    TyBool = 200,
    TyByte,
    TyChar,
    TyDouble,
    TyFloat,
    TyInt,
    TyInt8,
    TyInt16,
    TyInt32,
    TyInt64,
    TyIsize,
    TyString,
    TyStr,
    TyUInt8,
    TyUInt16,
    TyUInt32,
    TyUInt64,
    TyUsize,

    // --- Literals (300–319) ---
    IntegerLiteral = 300,
    FloatLiteral,
    BooleanLiteral,
    CharacterLiteral,
    StringLiteral,
    RawStringLiteral,
    MultilineStringLiteral,

    // --- Assignment operators (400–419) ---
    OpAssign = 400,
    OpPlusAssign,
    OpMinusAssign,
    OpStarAssign,
    OpSlashAssign,
    OpPercentAssign,
    OpAmpAssign,
    OpPipeAssign,
    OpCaretAssign,
    OpLessLessAssign,
    OpGreaterGreaterAssign,

    // --- Comparison operators (420–439) ---
    OpEqual = 420,
    OpNotEqual,
    OpLess,
    OpLessEqual,
    OpGreater,
    OpGreaterEqual,
    OpSpaceship,

    // --- Logical operators (440–459) ---
    OpLogicalAnd = 440,
    OpLogicalOr,
    OpLogicalNot,

    // --- Bitwise operators (460–479) ---
    OpAmp = 460,
    OpPipe,
    OpCaret,
    OpTilde,
    OpLessLess,
    OpGreaterGreater,

    // --- Arithmetic operators (480–499) ---
    OpPlus = 480,
    OpMinus,
    OpStar,
    OpSlash,
    OpPercent,

    // --- Increment / decrement (500–509) ---
    OpPlusPlus = 500,
    OpMinusMinus,

    // --- Other operators (510–549) ---
    OpArrow = 510,
    OpFatArrow,
    OpColonColon,
    OpDot,
    OpDotDot,
    OpDotDotDot,
    OpQuestion,
    OpQuestionQuestion,
    OpAt,
    OpHash,
    OpDollar,

    // --- Punctuation (550–579) ---
    PunctComma = 550,
    PunctSemicolon,
    PunctColon,
    PunctBang,
    PunctEqual,

    // --- Delimiters (580–599) ---
    DelimLeftParen = 580,
    DelimRightParen,
    DelimLeftBrace,
    DelimRightBrace,
    DelimLeftBracket,
    DelimRightBracket,
    DelimLeftAngle,
    DelimRightAngle,

    // --- Attributes & annotations (600–619) ---
    AttrStart = 600,
    Annotation,

    // --- Compiler directives (620–639) ---
    DirectiveHash = 620,
    DirectiveInclude,
    DirectiveDefine,
    DirectiveIf,
    DirectiveElse,
    DirectiveEndif,
    DirectivePragma,

    // --- Trivia (700–709) ---
    Comment = 700,
    Whitespace,
    Newline,
};

[[nodiscard]] constexpr bool IsKeyword(TokenType type) noexcept {
    const auto value = static_cast<std::uint16_t>(type);
    return value >= static_cast<std::uint16_t>(TokenType::KwAlignof) &&
           value < static_cast<std::uint16_t>(TokenType::TyBool);
}

[[nodiscard]] constexpr bool IsPrimitiveType(TokenType type) noexcept {
    const auto value = static_cast<std::uint16_t>(type);
    return value >= static_cast<std::uint16_t>(TokenType::TyBool) &&
           value < static_cast<std::uint16_t>(TokenType::IntegerLiteral);
}

[[nodiscard]] constexpr bool IsLiteral(TokenType type) noexcept {
    const auto value = static_cast<std::uint16_t>(type);
    return value >= static_cast<std::uint16_t>(TokenType::IntegerLiteral) &&
           value < static_cast<std::uint16_t>(TokenType::OpAssign);
}

[[nodiscard]] constexpr bool IsOperator(TokenType type) noexcept {
    const auto value = static_cast<std::uint16_t>(type);
    return value >= static_cast<std::uint16_t>(TokenType::OpAssign) &&
           value < static_cast<std::uint16_t>(TokenType::PunctComma);
}

[[nodiscard]] constexpr bool IsPunctuation(TokenType type) noexcept {
    const auto value = static_cast<std::uint16_t>(type);
    return value >= static_cast<std::uint16_t>(TokenType::PunctComma) &&
           value < static_cast<std::uint16_t>(TokenType::DelimLeftParen);
}

[[nodiscard]] constexpr bool IsDelimiter(TokenType type) noexcept {
    const auto value = static_cast<std::uint16_t>(type);
    return value >= static_cast<std::uint16_t>(TokenType::DelimLeftParen) &&
           value < static_cast<std::uint16_t>(TokenType::AttrStart);
}

[[nodiscard]] constexpr bool IsDirective(TokenType type) noexcept {
    const auto value = static_cast<std::uint16_t>(type);
    return value >= static_cast<std::uint16_t>(TokenType::DirectiveHash) &&
           value < static_cast<std::uint16_t>(TokenType::Comment);
}

[[nodiscard]] constexpr bool IsTrivia(TokenType type) noexcept {
    const auto value = static_cast<std::uint16_t>(type);
    return value >= static_cast<std::uint16_t>(TokenType::Comment) &&
           value <= static_cast<std::uint16_t>(TokenType::Newline);
}

[[nodiscard]] constexpr bool IsEndOfFile(TokenType type) noexcept {
    return type == TokenType::EndOfFile;
}

[[nodiscard]] constexpr bool IsError(TokenType type) noexcept {
    return type == TokenType::Error;
}

[[nodiscard]] std::string_view ToString(TokenType type) noexcept;

}  // namespace myc::lexer
