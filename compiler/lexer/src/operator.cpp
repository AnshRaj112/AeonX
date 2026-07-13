#include "myc/lexer/operator.hpp"

#include <algorithm>
#include <array>

namespace myc::lexer {

namespace {

constexpr Operator MakeOperator(std::string_view symbol, TokenType type, int precedence,
                                Associativity associativity, OperatorArity arity,
                                std::uint32_t overload_set_id = 0) {
    return Operator{symbol, type, precedence, associativity, arity, overload_set_id};
}

constexpr std::array kOperators = {
    MakeOperator("!=", TokenType::OpNotEqual, 7, Associativity::Left, OperatorArity::Binary),
    MakeOperator("#", TokenType::OpHash, 1, Associativity::None, OperatorArity::Unary),
    MakeOperator("$", TokenType::OpDollar, 1, Associativity::None, OperatorArity::Unary),
    MakeOperator("%", TokenType::OpPercent, 3, Associativity::Left, OperatorArity::Binary),
    MakeOperator("%=", TokenType::OpPercentAssign, 2, Associativity::Right, OperatorArity::Binary),
    MakeOperator("&", TokenType::OpAmp, 8, Associativity::Left, OperatorArity::UnaryOrBinary),
    MakeOperator("&&", TokenType::OpLogicalAnd, 11, Associativity::Left, OperatorArity::Binary),
    MakeOperator("&=", TokenType::OpAmpAssign, 2, Associativity::Right, OperatorArity::Binary),
    MakeOperator("*", TokenType::OpStar, 3, Associativity::Left, OperatorArity::Binary),
    MakeOperator("*=", TokenType::OpStarAssign, 2, Associativity::Right, OperatorArity::Binary),
    MakeOperator("+", TokenType::OpPlus, 4, Associativity::Left, OperatorArity::UnaryOrBinary),
    MakeOperator("++", TokenType::OpPlusPlus, 3, Associativity::None, OperatorArity::Unary),
    MakeOperator("+=", TokenType::OpPlusAssign, 2, Associativity::Right, OperatorArity::Binary),
    MakeOperator("-", TokenType::OpMinus, 4, Associativity::Left, OperatorArity::UnaryOrBinary),
    MakeOperator("--", TokenType::OpMinusMinus, 3, Associativity::None, OperatorArity::Unary),
    MakeOperator("-=", TokenType::OpMinusAssign, 2, Associativity::Right, OperatorArity::Binary),
    MakeOperator("->", TokenType::OpArrow, 1, Associativity::Right, OperatorArity::Binary),
    MakeOperator(".", TokenType::OpDot, 1, Associativity::Left, OperatorArity::Binary),
    MakeOperator("..", TokenType::OpDotDot, 1, Associativity::Left, OperatorArity::Binary),
    MakeOperator("...", TokenType::OpDotDotDot, 1, Associativity::Left, OperatorArity::Binary),
    MakeOperator("/", TokenType::OpSlash, 3, Associativity::Left, OperatorArity::Binary),
    MakeOperator("/=", TokenType::OpSlashAssign, 2, Associativity::Right, OperatorArity::Binary),
    MakeOperator("::", TokenType::OpColonColon, 1, Associativity::None, OperatorArity::Binary),
    MakeOperator("<", TokenType::OpLess, 6, Associativity::Left, OperatorArity::Binary),
    MakeOperator("<<", TokenType::OpLessLess, 5, Associativity::Left, OperatorArity::Binary),
    MakeOperator("<<=", TokenType::OpLessLessAssign, 2, Associativity::Right,
                 OperatorArity::Binary),
    MakeOperator("<=", TokenType::OpLessEqual, 6, Associativity::Left, OperatorArity::Binary),
    MakeOperator("<=>", TokenType::OpSpaceship, 6, Associativity::Left, OperatorArity::Binary),
    MakeOperator("=", TokenType::OpAssign, 2, Associativity::Right, OperatorArity::Binary),
    MakeOperator("==", TokenType::OpEqual, 7, Associativity::Left, OperatorArity::Binary),
    MakeOperator(">", TokenType::OpGreater, 6, Associativity::Left, OperatorArity::Binary),
    MakeOperator(">=", TokenType::OpGreaterEqual, 6, Associativity::Left, OperatorArity::Binary),
    MakeOperator(">>", TokenType::OpGreaterGreater, 5, Associativity::Left, OperatorArity::Binary),
    MakeOperator(">>=", TokenType::OpGreaterGreaterAssign, 2, Associativity::Right,
                 OperatorArity::Binary),
    MakeOperator("?", TokenType::OpQuestion, 13, Associativity::Right, OperatorArity::Unary),
    MakeOperator("??", TokenType::OpQuestionQuestion, 13, Associativity::Right,
                 OperatorArity::Binary),
    MakeOperator("@", TokenType::OpAt, 1, Associativity::None, OperatorArity::Unary),
    MakeOperator("^", TokenType::OpCaret, 9, Associativity::Left, OperatorArity::Binary),
    MakeOperator("^=", TokenType::OpCaretAssign, 2, Associativity::Right, OperatorArity::Binary),
    MakeOperator("|", TokenType::OpPipe, 10, Associativity::Left, OperatorArity::UnaryOrBinary),
    MakeOperator("|=", TokenType::OpPipeAssign, 2, Associativity::Right, OperatorArity::Binary),
    MakeOperator("||", TokenType::OpLogicalOr, 12, Associativity::Left, OperatorArity::Binary),
    MakeOperator("~", TokenType::OpTilde, 3, Associativity::None, OperatorArity::Unary),
};

static_assert(
    std::is_sorted(kOperators.begin(), kOperators.end(),
                   [](const Operator& lhs, const Operator& rhs) { return lhs.symbol < rhs.symbol; }),
    "operator table must remain lexicographically sorted");

}  // namespace

const Operator* OperatorTable::Lookup(std::string_view symbol) noexcept {
    const auto* begin = kOperators.begin();
    const auto* end = kOperators.end();
    const auto* found = std::lower_bound(
        begin, end, symbol,
        [](const Operator& op, std::string_view value) { return op.symbol < value; });
    if (found != end && found->symbol == symbol) {
        return found;
    }
    return nullptr;
}

std::optional<TokenType> OperatorTable::LookupType(std::string_view symbol) noexcept {
    if (const Operator* op = Lookup(symbol)) {
        return op->type;
    }
    return std::nullopt;
}

bool OperatorTable::IsOperatorSymbol(std::string_view symbol) noexcept {
    return Lookup(symbol) != nullptr;
}

std::size_t OperatorTable::Count() noexcept { return kOperators.size(); }

const Operator& OperatorTable::At(std::size_t index) noexcept { return kOperators.at(index); }

}  // namespace myc::lexer
