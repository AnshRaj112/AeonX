#pragma once

#include "myc/lexer/token_type.hpp"

#include <cstdint>
#include <optional>
#include <string_view>

namespace myc::lexer {

enum class Associativity : std::uint8_t {
    None,
    Left,
    Right,
};

enum class OperatorArity : std::uint8_t {
    Unary,
    Binary,
    UnaryOrBinary,
};

/// Metadata for an operator symbol.
struct Operator {
    std::string_view symbol;
    TokenType type;
    int precedence;
    Associativity associativity;
    OperatorArity arity;
    std::uint32_t overload_set_id;

    [[nodiscard]] constexpr bool IsUnary() const noexcept {
        return arity == OperatorArity::Unary || arity == OperatorArity::UnaryOrBinary;
    }

    [[nodiscard]] constexpr bool IsBinary() const noexcept {
        return arity == OperatorArity::Binary || arity == OperatorArity::UnaryOrBinary;
    }

    [[nodiscard]] constexpr bool HasOverloadSet() const noexcept { return overload_set_id != 0; }
};

/// Centralized operator table with precedence and associativity.
class OperatorTable {
public:
    [[nodiscard]] static const Operator* Lookup(std::string_view symbol) noexcept;
    [[nodiscard]] static std::optional<TokenType> LookupType(std::string_view symbol) noexcept;
    [[nodiscard]] static bool IsOperatorSymbol(std::string_view symbol) noexcept;
    [[nodiscard]] static std::size_t Count() noexcept;
    [[nodiscard]] static const Operator& At(std::size_t index) noexcept;
};

}  // namespace myc::lexer
