#include "myc/lexer/keyword.hpp"

#include <algorithm>
#include <array>

namespace myc::lexer {

namespace {

constexpr Keyword MakeKeyword(std::string_view text, TokenType type, bool reserved = true,
                              std::uint32_t version = 1, std::uint64_t feature_gate = 0) {
    return Keyword{text, type, reserved, version, feature_gate};
}

constexpr std::array kKeywords = {
    MakeKeyword("Self", TokenType::KwSelfType),
    MakeKeyword("alignof", TokenType::KwAlignof),
    MakeKeyword("and", TokenType::KwAnd),
    MakeKeyword("as", TokenType::KwAs),
    MakeKeyword("async", TokenType::KwAsync),
    MakeKeyword("await", TokenType::KwAwait),
    MakeKeyword("bool", TokenType::TyBool),
    MakeKeyword("break", TokenType::KwBreak),
    MakeKeyword("byte", TokenType::TyByte),
    MakeKeyword("case", TokenType::KwCase),
    MakeKeyword("catch", TokenType::KwCatch),
    MakeKeyword("char", TokenType::TyChar),
    MakeKeyword("class", TokenType::KwClass),
    MakeKeyword("const", TokenType::KwConst),
    MakeKeyword("continue", TokenType::KwContinue),
    MakeKeyword("default", TokenType::KwDefault),
    MakeKeyword("defer", TokenType::KwDefer),
    MakeKeyword("double", TokenType::TyDouble),
    MakeKeyword("else", TokenType::KwElse),
    MakeKeyword("enum", TokenType::KwEnum),
    MakeKeyword("extern", TokenType::KwExtern),
    MakeKeyword("false", TokenType::KwFalse),
    MakeKeyword("float", TokenType::TyFloat),
    MakeKeyword("fn", TokenType::KwFn),
    MakeKeyword("for", TokenType::KwFor),
    MakeKeyword("func", TokenType::KwFn),
    MakeKeyword("i16", TokenType::TyInt16),
    MakeKeyword("i32", TokenType::TyInt32),
    MakeKeyword("i64", TokenType::TyInt64),
    MakeKeyword("i8", TokenType::TyInt8),
    MakeKeyword("if", TokenType::KwIf),
    MakeKeyword("impl", TokenType::KwImpl),
    MakeKeyword("import", TokenType::KwImport),
    MakeKeyword("in", TokenType::KwIn),
    MakeKeyword("int", TokenType::TyInt),
    MakeKeyword("is", TokenType::KwIs),
    MakeKeyword("isize", TokenType::TyIsize),
    MakeKeyword("let", TokenType::KwLet),
    MakeKeyword("loop", TokenType::KwLoop),
    MakeKeyword("match", TokenType::KwMatch),
    MakeKeyword("mod", TokenType::KwMod),
    MakeKeyword("module", TokenType::KwMod),
    MakeKeyword("mut", TokenType::KwMut),
    MakeKeyword("nil", TokenType::KwNil),
    MakeKeyword("not", TokenType::KwNot),
    MakeKeyword("or", TokenType::KwOr),
    MakeKeyword("override", TokenType::KwOverride),
    MakeKeyword("package", TokenType::KwPackage),
    MakeKeyword("priv", TokenType::KwPriv),
    MakeKeyword("pub", TokenType::KwPub),
    MakeKeyword("return", TokenType::KwReturn),
    MakeKeyword("self", TokenType::KwSelf),
    MakeKeyword("sizeof", TokenType::KwSizeof),
    MakeKeyword("static", TokenType::KwStatic),
    MakeKeyword("str", TokenType::TyStr),
    MakeKeyword("string", TokenType::TyString),
    MakeKeyword("struct", TokenType::KwStruct),
    MakeKeyword("super", TokenType::KwSuper),
    MakeKeyword("throw", TokenType::KwThrow),
    MakeKeyword("trait", TokenType::KwTrait),
    MakeKeyword("true", TokenType::KwTrue),
    MakeKeyword("try", TokenType::KwTry),
    MakeKeyword("typeof", TokenType::KwTypeof),
    MakeKeyword("u16", TokenType::TyUInt16),
    MakeKeyword("u32", TokenType::TyUInt32),
    MakeKeyword("u64", TokenType::TyUInt64),
    MakeKeyword("u8", TokenType::TyUInt8),
    MakeKeyword("unsafe", TokenType::KwUnsafe),
    MakeKeyword("use", TokenType::KwUse),
    MakeKeyword("usize", TokenType::TyUsize),
    MakeKeyword("virtual", TokenType::KwVirtual),
    MakeKeyword("void", TokenType::KwVoid),
    MakeKeyword("where", TokenType::KwWhere),
    MakeKeyword("while", TokenType::KwWhile),
    MakeKeyword("xor", TokenType::KwXor),
    MakeKeyword("yield", TokenType::KwYield),
};

static_assert(
    std::is_sorted(kKeywords.begin(), kKeywords.end(),
                   [](const Keyword& lhs, const Keyword& rhs) { return lhs.text < rhs.text; }),
    "keyword table must remain lexicographically sorted");

}  // namespace

const Keyword* KeywordTable::Lookup(std::string_view text) noexcept {
    const auto* begin = kKeywords.begin();
    const auto* end = kKeywords.end();
    const auto* found = std::lower_bound(
        begin, end, text,
        [](const Keyword& keyword, std::string_view value) { return keyword.text < value; });
    if (found != end && found->text == text) {
        return found;
    }
    return nullptr;
}

std::optional<TokenType> KeywordTable::LookupType(std::string_view text) noexcept {
    if (const Keyword* keyword = Lookup(text)) {
        return keyword->type;
    }
    return std::nullopt;
}

bool KeywordTable::IsKeywordText(std::string_view text) noexcept { return Lookup(text) != nullptr; }

std::size_t KeywordTable::Count() noexcept { return kKeywords.size(); }

const Keyword& KeywordTable::At(std::size_t index) noexcept { return kKeywords.at(index); }

}  // namespace myc::lexer
