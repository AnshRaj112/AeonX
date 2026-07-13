#include "myc_test.hpp"

#include "myc/diagnostics/diagnostic_emitter.hpp"
#include "myc/diagnostics/diagnostic_engine.hpp"
#include "myc/lexer/lexer.hpp"
#include "myc/lexer/lexer_options.hpp"
#include "myc/lexer/token_type.hpp"
#include "myc/source/source_manager.hpp"

#include <chrono>
#include <string>
#include <string_view>
#include <vector>

namespace {

using myc::lexer::Lexer;
using myc::lexer::LexerOptions;
using myc::lexer::LexerResult;
using myc::lexer::TokenType;

class LexerHarness {
public:
    [[nodiscard]] LexerResult Tokenize(std::string_view source,
                                       LexerOptions options = {}) const {
        const myc::FileId id =
            source_manager_.AddMemoryBuffer("test.myc", std::string(source));
        const myc::source::SourceBuffer* buffer = source_manager_.GetBuffer(id);
        MYC_ASSERT(buffer != nullptr);
        Lexer lexer(*buffer, source_manager_, emitter_, options);
        return lexer.Tokenize();
    }

    [[nodiscard]] myc::diagnostics::DiagnosticEngine& GetEngine() { return engine_; }

private:
    mutable myc::source::SourceManager source_manager_;
    mutable myc::diagnostics::DiagnosticEngine engine_;
    mutable myc::diagnostics::DiagnosticEmitter emitter_{engine_};
};

void ExpectTypes(const LexerResult& result, std::initializer_list<TokenType> types) {
    std::size_t index = 0;
    for (const TokenType type : types) {
        MYC_ASSERT(result.tokens.Peek(index).GetType() == type);
        ++index;
    }
    MYC_ASSERT(result.tokens.Peek(index).IsEOF());
    MYC_ASSERT_EQ(result.tokens.Size(), index + 1);
}

void ExpectLexeme(const LexerResult& result, std::size_t index, std::string_view lexeme) {
    MYC_ASSERT_EQ(result.tokens.Peek(index).GetLexeme(), lexeme);
}

void ExpectNoErrors(const LexerResult& result) { MYC_ASSERT(!result.had_errors); }

void ExpectErrors(const LexerResult& result, std::size_t count) {
    MYC_ASSERT(result.had_errors);
    MYC_ASSERT_EQ(result.statistics.GetErrorsEncountered(), count);
}

}  // namespace

#define LEXER_TYPES(name, source, ...)                    \
    MYC_TEST(lexer_prod_##name) {                           \
        const LexerHarness harness;                         \
        const LexerResult result = harness.Tokenize(source); \
        ExpectNoErrors(result);                             \
        ExpectTypes(result, {__VA_ARGS__});                 \
    }

#define LEXER_TYPES_ERR(name, source, errors, ...)          \
    MYC_TEST(lexer_prod_##name) {                           \
        const LexerHarness harness;                         \
        const LexerResult result = harness.Tokenize(source);  \
        ExpectErrors(result, errors);                         \
        ExpectTypes(result, {__VA_ARGS__});                   \
    }

#define LEXER_LEXEME(name, source, index, lexeme, type)    \
    MYC_TEST(lexer_prod_##name) {                           \
        const LexerHarness harness;                         \
        const LexerResult result = harness.Tokenize(source); \
        ExpectNoErrors(result);                             \
        MYC_ASSERT(result.tokens.Peek(index).GetType() == type); \
        ExpectLexeme(result, index, lexeme);                 \
    }

// --- Identifiers (25) ---
LEXER_TYPES(id_hello, "hello", TokenType::Identifier)
LEXER_TYPES(id_userName, "userName", TokenType::Identifier)
LEXER_TYPES(id_underscore, "_age", TokenType::Identifier)
LEXER_TYPES(id_counter1, "counter1", TokenType::Identifier)
LEXER_TYPES(id_graph, "Graph", TokenType::Identifier)
LEXER_TYPES(id_segment_tree, "SegmentTree", TokenType::Identifier)
LEXER_TYPES(id_snake, "snake_case_name", TokenType::Identifier)
LEXER_TYPES(id_mixed, "fooBar123", TokenType::Identifier)
LEXER_TYPES(id_single_underscore, "_", TokenType::Identifier)
LEXER_TYPES(id_a, "a", TokenType::Identifier)
LEXER_TYPES(id_z, "z", TokenType::Identifier)
LEXER_TYPES(id_long, "veryLongIdentifierNameForTesting", TokenType::Identifier)
LEXER_TYPES(id_trailing_digit, "value9", TokenType::Identifier)
LEXER_TYPES(id_embedded_digit, "x1y2z3", TokenType::Identifier)
LEXER_TYPES(id_upper, "UPPER", TokenType::Identifier)
LEXER_TYPES(id_lower, "lower", TokenType::Identifier)
LEXER_TYPES(id_camel, "camelCase", TokenType::Identifier)
LEXER_TYPES(id_pascal, "PascalCase", TokenType::Identifier)
LEXER_TYPES(id_with_underscores, "a_b_c", TokenType::Identifier)
LEXER_TYPES(id_start_underscore_digit, "_1abc", TokenType::Identifier)
LEXER_TYPES(id_unicode_placeholder, "caf\u00E9", TokenType::Identifier)
LEXER_TYPES(id_two, "aa bb", TokenType::Identifier, TokenType::Identifier)
LEXER_TYPES(id_after_paren, "(hello)", TokenType::DelimLeftParen, TokenType::Identifier,
            TokenType::DelimRightParen)
LEXER_TYPES(id_after_comma, "a,b", TokenType::Identifier, TokenType::PunctComma,
            TokenType::Identifier)
LEXER_TYPES(id_not_keyword_prefix, "iffy", TokenType::Identifier)

// --- Keywords (35) ---
LEXER_TYPES(kw_if, "if", TokenType::KwIf)
LEXER_TYPES(kw_else, "else", TokenType::KwElse)
LEXER_TYPES(kw_while, "while", TokenType::KwWhile)
LEXER_TYPES(kw_for, "for", TokenType::KwFor)
LEXER_TYPES(kw_return, "return", TokenType::KwReturn)
LEXER_TYPES(kw_fn, "fn", TokenType::KwFn)
LEXER_TYPES(kw_class, "class", TokenType::KwClass)
LEXER_TYPES(kw_struct, "struct", TokenType::KwStruct)
LEXER_TYPES(kw_enum, "enum", TokenType::KwEnum)
LEXER_TYPES(kw_match, "match", TokenType::KwMatch)
LEXER_TYPES(kw_import, "import", TokenType::KwImport)
LEXER_TYPES(kw_mod, "mod", TokenType::KwMod)
LEXER_TYPES(kw_package, "package", TokenType::KwPackage)
LEXER_TYPES(kw_unsafe, "unsafe", TokenType::KwUnsafe)
LEXER_TYPES(kw_async, "async", TokenType::KwAsync)
LEXER_TYPES(kw_await, "await", TokenType::KwAwait)
LEXER_TYPES(kw_let, "let", TokenType::KwLet)
LEXER_TYPES(kw_mut, "mut", TokenType::KwMut)
LEXER_TYPES(kw_pub, "pub", TokenType::KwPub)
LEXER_TYPES(kw_true, "true", TokenType::KwTrue)
LEXER_TYPES(kw_false, "false", TokenType::KwFalse)
LEXER_TYPES(kw_nil, "nil", TokenType::KwNil)
LEXER_TYPES(kw_break, "break", TokenType::KwBreak)
LEXER_TYPES(kw_continue, "continue", TokenType::KwContinue)
LEXER_TYPES(kw_try, "try", TokenType::KwTry)
LEXER_TYPES(kw_catch, "catch", TokenType::KwCatch)
LEXER_TYPES(kw_throw, "throw", TokenType::KwThrow)
LEXER_TYPES(kw_impl, "impl", TokenType::KwImpl)
LEXER_TYPES(kw_self, "self", TokenType::KwSelf)
LEXER_TYPES(kw_super, "super", TokenType::KwSuper)
LEXER_TYPES(kw_type_bool, "bool", TokenType::TyBool)
LEXER_TYPES(kw_type_int, "int", TokenType::TyInt)
MYC_TEST(lexer_prod_kw_type_string) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("string");
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).GetType() == TokenType::TyString);
    MYC_ASSERT(result.tokens.Peek(0).GetLexeme() == "string");
    MYC_ASSERT(result.tokens.Peek(0).IsPrimitiveType());
}
LEXER_TYPES(kw_type_void, "void", TokenType::KwVoid)
LEXER_TYPES(kw_sequence, "if else while", TokenType::KwIf, TokenType::KwElse, TokenType::KwWhile)

// --- Numbers (40) ---
LEXER_TYPES(num_10, "10", TokenType::IntegerLiteral)
LEXER_TYPES(num_0, "0", TokenType::IntegerLiteral)
LEXER_TYPES(num_hex_ff, "0xFF", TokenType::IntegerLiteral)
LEXER_TYPES(num_hex_lower, "0x1a2b", TokenType::IntegerLiteral)
LEXER_TYPES(num_binary, "0b1011", TokenType::IntegerLiteral)
LEXER_TYPES(num_octal, "0o755", TokenType::IntegerLiteral)
LEXER_TYPES(num_float_pi, "3.1415", TokenType::FloatLiteral)
LEXER_TYPES(num_float_leading_zero, "0.5", TokenType::FloatLiteral)
LEXER_TYPES(num_scientific_e9, "1e9", TokenType::FloatLiteral)
LEXER_TYPES(num_scientific_neg, "2.5e-4", TokenType::FloatLiteral)
LEXER_TYPES(num_scientific_pos_exp, "6e+10", TokenType::FloatLiteral)
LEXER_TYPES(num_scientific_upper, "1E3", TokenType::FloatLiteral)
LEXER_TYPES(num_trailing_dot, "123.", TokenType::IntegerLiteral, TokenType::OpDot)
LEXER_TYPES(num_hex_sequence, "0x00 0xFF", TokenType::IntegerLiteral, TokenType::IntegerLiteral)
LEXER_TYPES(num_binary_sequence, "0b1 0b0", TokenType::IntegerLiteral, TokenType::IntegerLiteral)
LEXER_TYPES(num_large, "999999999999", TokenType::IntegerLiteral)
LEXER_TYPES(num_mixed_expr, "1+2", TokenType::IntegerLiteral, TokenType::OpPlus,
            TokenType::IntegerLiteral)
LEXER_TYPES(num_float_add, "1.0+2.0", TokenType::FloatLiteral, TokenType::OpPlus,
            TokenType::FloatLiteral)
LEXER_TYPES(num_dot_prefixed, ".5", TokenType::FloatLiteral)
LEXER_TYPES(num_0x_dead, "0xDEAD", TokenType::IntegerLiteral)
LEXER_TYPES(num_0b_1111, "0b11110000", TokenType::IntegerLiteral)
LEXER_TYPES(num_0o_10, "0o10", TokenType::IntegerLiteral)
LEXER_TYPES(num_exp_only, "5e2", TokenType::FloatLiteral)
LEXER_TYPES(num_int_then_id, "1 to", TokenType::IntegerLiteral, TokenType::Identifier)
LEXER_TYPES_ERR(num_hex_no_digits, "0x", 1, TokenType::Error)
LEXER_TYPES_ERR(num_bin_no_digits, "0b", 1, TokenType::Error)
LEXER_TYPES_ERR(num_oct_no_digits, "0o", 1, TokenType::Error)
LEXER_TYPES_ERR(num_bad_suffix, "123abc", 1, TokenType::Error, TokenType::Identifier)
LEXER_TYPES(num_double_exp, "1.2e-3", TokenType::FloatLiteral)
LEXER_TYPES(num_leading_zeros, "007", TokenType::IntegerLiteral)
LEXER_TYPES(num_negative_not_number, "-1", TokenType::OpMinus, TokenType::IntegerLiteral)
LEXER_TYPES(num_in_parens, "(42)", TokenType::DelimLeftParen, TokenType::IntegerLiteral,
            TokenType::DelimRightParen)
LEXER_TYPES(num_comma_list, "1,2,3", TokenType::IntegerLiteral, TokenType::PunctComma,
            TokenType::IntegerLiteral, TokenType::PunctComma, TokenType::IntegerLiteral)
LEXER_TYPES(num_float_trailing, "2.", TokenType::IntegerLiteral, TokenType::OpDot)
LEXER_TYPES(num_scientific_no_mantissa, "e 10", TokenType::Identifier, TokenType::IntegerLiteral)
LEXER_TYPES(num_hex_in_expr, "0x10+1", TokenType::IntegerLiteral, TokenType::OpPlus,
            TokenType::IntegerLiteral)
LEXER_TYPES(num_binary_in_expr, "0b10-1", TokenType::IntegerLiteral, TokenType::OpMinus,
            TokenType::IntegerLiteral)
LEXER_TYPES(num_oct_in_expr, "0o7*2", TokenType::IntegerLiteral, TokenType::OpStar,
            TokenType::IntegerLiteral)
LEXER_TYPES(num_many_digits, "12345678901234567890", TokenType::IntegerLiteral)
LEXER_TYPES(num_float_many_digits, "3.141592653589793", TokenType::FloatLiteral)

// --- Strings (25) ---
LEXER_TYPES(str_hello, "\"Hello\"", TokenType::StringLiteral)
LEXER_TYPES(str_empty, "\"\"", TokenType::StringLiteral)
LEXER_TYPES(str_escape_n, "\"Line\\nBreak\"", TokenType::StringLiteral)
LEXER_TYPES(str_escape_t, "\"tab\\there\"", TokenType::StringLiteral)
LEXER_TYPES(str_escape_quote, "\"say \\\"hi\\\"\"", TokenType::StringLiteral)
LEXER_TYPES(str_escape_backslash, "\"\\\\\"", TokenType::StringLiteral)
LEXER_TYPES(str_escape_null, "\"\\0\"", TokenType::StringLiteral)
LEXER_TYPES(str_escape_hex, "\"\\x41\"", TokenType::StringLiteral)
LEXER_TYPES(str_two, "\"a\" \"b\"", TokenType::StringLiteral, TokenType::StringLiteral)
LEXER_TYPES(str_with_spaces, "\"  spaced  \"", TokenType::StringLiteral)
LEXER_TYPES_ERR(str_unterminated, "\"oops", 1, TokenType::Error)
LEXER_TYPES_ERR(str_bad_escape, "\"\\q\"", 1, TokenType::Error, TokenType::StringLiteral)
MYC_TEST(lexer_prod_str_newline_inside) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("\"a\nb\"");
    MYC_ASSERT(result.had_errors);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::Error));
}

MYC_TEST(lexer_prod_ch_bad_escape) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("'\\q'");
    MYC_ASSERT(result.had_errors);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::Error));
}

MYC_TEST(lexer_prod_ch_newline) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("'\n'");
    MYC_ASSERT(result.had_errors);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::Error));
}
LEXER_TYPES(str_adjacent_id, "\"x\"y", TokenType::StringLiteral, TokenType::Identifier)
LEXER_TYPES(str_unicode_placeholder, "\"caf\u00E9\"", TokenType::StringLiteral)
LEXER_TYPES(str_only_escape, "\"\\n\"", TokenType::StringLiteral)
LEXER_TYPES(str_mixed_escapes, "\"a\\tb\\nc\"", TokenType::StringLiteral)
LEXER_TYPES(str_in_call, "f(\"hi\")", TokenType::Identifier, TokenType::DelimLeftParen,
            TokenType::StringLiteral, TokenType::DelimRightParen)
LEXER_TYPES(str_assign, "s=\"\"", TokenType::Identifier, TokenType::OpAssign,
            TokenType::StringLiteral)
LEXER_TYPES(str_semicolon, "\";\";", TokenType::StringLiteral, TokenType::PunctSemicolon)
LEXER_TYPES(str_hash_inside, "\"#\"", TokenType::StringLiteral)
LEXER_TYPES(str_at_inside, "\"@\"", TokenType::StringLiteral)
LEXER_TYPES(str_braces_inside, "\"{}\"", TokenType::StringLiteral)
LEXER_TYPES(str_brackets_inside, "\"[]\"", TokenType::StringLiteral)
LEXER_TYPES(str_parens_inside, "\"()\"", TokenType::StringLiteral)

// --- Characters (15) ---
LEXER_TYPES(ch_a, "'a'", TokenType::CharacterLiteral)
LEXER_TYPES(ch_escape_n, "'\\n'", TokenType::CharacterLiteral)
LEXER_TYPES(ch_escape_t, "'\\t'", TokenType::CharacterLiteral)
LEXER_TYPES(ch_escape_hex, "'\\x41'", TokenType::CharacterLiteral)
LEXER_TYPES(ch_escape_quote, "'\\''", TokenType::CharacterLiteral)
LEXER_TYPES_ERR(ch_unterminated, "'a", 1, TokenType::Error)
LEXER_TYPES_ERR(ch_empty, "''", 1, TokenType::Error)
LEXER_TYPES(ch_two, "'a' 'b'", TokenType::CharacterLiteral, TokenType::CharacterLiteral)
LEXER_TYPES(ch_in_expr, "x+'a'", TokenType::Identifier, TokenType::OpPlus,
            TokenType::CharacterLiteral)
LEXER_TYPES(ch_zero, "'\\0'", TokenType::CharacterLiteral)
LEXER_TYPES(ch_backslash, "'\\\\'", TokenType::CharacterLiteral)
LEXER_TYPES(ch_unicode_placeholder, "'\u00E9'", TokenType::CharacterLiteral)
LEXER_TYPES(ch_before_semi, "'a';", TokenType::CharacterLiteral, TokenType::PunctSemicolon)

// --- Comments (15) ---
MYC_TEST(lexer_prod_comment_line_skipped) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("// comment\nlet");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::KwLet});
}

MYC_TEST(lexer_prod_comment_block_skipped) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("/* block */let");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::KwLet});
}

MYC_TEST(lexer_prod_comment_nested_block) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("/* a /* b */ c */x");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::Identifier});
    ExpectLexeme(result, 0, "x");
}

MYC_TEST(lexer_prod_comment_line_after_code) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("let x = 1; // end");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::KwLet, TokenType::Identifier, TokenType::OpAssign,
                         TokenType::IntegerLiteral, TokenType::PunctSemicolon});
}

MYC_TEST(lexer_prod_comment_block_multiline) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("/* one\ntwo */fn");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::KwFn});
}

MYC_TEST(lexer_prod_comment_unterminated_block) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("/* oops");
    ExpectErrors(result, 1);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::Error));
}

MYC_TEST(lexer_prod_comment_trivia_mode_line) {
    LexerOptions options;
    options.include_trivia = true;
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("//x\ny", options);
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::Comment));
    MYC_ASSERT(result.tokens.Peek(1).Is(TokenType::Newline));
    MYC_ASSERT(result.tokens.Peek(2).Is(TokenType::Identifier));
}

MYC_TEST(lexer_prod_comment_trivia_mode_block) {
    LexerOptions options;
    options.include_trivia = true;
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("/*a*/b", options);
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::Comment));
    MYC_ASSERT(result.tokens.Peek(1).Is(TokenType::Identifier));
}

MYC_TEST(lexer_prod_comment_not_divide) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("1/2");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::IntegerLiteral, TokenType::OpSlash, TokenType::IntegerLiteral});
}

MYC_TEST(lexer_prod_comment_slash_without_star) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("/=");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::OpSlashAssign});
}

MYC_TEST(lexer_prod_comment_doc_placeholder) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("/// doc\nlet");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::KwLet});
}

MYC_TEST(lexer_prod_comment_after_string) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("\"a\"//b\n1");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::StringLiteral, TokenType::IntegerLiteral});
}

MYC_TEST(lexer_prod_comment_star_without_slash) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("*2");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::OpStar, TokenType::IntegerLiteral});
}

MYC_TEST(lexer_prod_comment_inside_block_many_stars) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("/**/z");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::Identifier});
}

MYC_TEST(lexer_prod_comment_line_before_eof) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("// only");
    ExpectNoErrors(result);
    ExpectTypes(result, {});
}

// --- Whitespace (12) ---
MYC_TEST(lexer_prod_whitespace_skipped_between_tokens) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("  let   x  ");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::KwLet, TokenType::Identifier});
}

MYC_TEST(lexer_prod_whitespace_tabs) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("\tlet\tx\t");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::KwLet, TokenType::Identifier});
}

MYC_TEST(lexer_prod_whitespace_newlines) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("a\nb\nc");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::Identifier, TokenType::Identifier, TokenType::Identifier});
}

MYC_TEST(lexer_prod_whitespace_mixed) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize(" \t\r\n let");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::KwLet});
}

MYC_TEST(lexer_prod_whitespace_trivia_mode) {
    LexerOptions options;
    options.include_trivia = true;
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize(" a", options);
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::Whitespace));
    MYC_ASSERT(result.tokens.Peek(1).Is(TokenType::Identifier));
}

MYC_TEST(lexer_prod_whitespace_trivia_newline) {
    LexerOptions options;
    options.include_trivia = true;
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("\nlet", options);
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::Newline));
    MYC_ASSERT(result.tokens.Peek(1).Is(TokenType::KwLet));
}

MYC_TEST(lexer_prod_whitespace_only) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("   \n\t");
    ExpectNoErrors(result);
    ExpectTypes(result, {});
}

MYC_TEST(lexer_prod_whitespace_preserves_columns) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("  x");
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).GetLocation().GetColumn() > 1u);
}

MYC_TEST(lexer_prod_whitespace_between_operators) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("1  +  2");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::IntegerLiteral, TokenType::OpPlus, TokenType::IntegerLiteral});
}

MYC_TEST(lexer_prod_whitespace_crlf) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("a\r\nb");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::Identifier, TokenType::Identifier});
}

MYC_TEST(lexer_prod_whitespace_many_blank_lines) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("\n\n\nx");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::Identifier});
}

MYC_TEST(lexer_prod_whitespace_before_eof) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("x  ");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::Identifier});
}

// --- Operators (45) ---
LEXER_TYPES(op_plus, "+", TokenType::OpPlus)
LEXER_TYPES(op_minus, "-", TokenType::OpMinus)
LEXER_TYPES(op_star, "*", TokenType::OpStar)
LEXER_TYPES(op_slash, "/", TokenType::OpSlash)
LEXER_TYPES(op_percent, "%", TokenType::OpPercent)
LEXER_TYPES(op_assign, "=", TokenType::OpAssign)
LEXER_TYPES(op_equal, "==", TokenType::OpEqual)
LEXER_TYPES(op_not_equal, "!=", TokenType::OpNotEqual)
LEXER_TYPES(op_less, "<", TokenType::OpLess)
LEXER_TYPES(op_greater, ">", TokenType::OpGreater)
LEXER_TYPES(op_less_equal, "<=", TokenType::OpLessEqual)
LEXER_TYPES(op_greater_equal, ">=", TokenType::OpGreaterEqual)
LEXER_TYPES(op_spaceship, "<=>", TokenType::OpSpaceship)
LEXER_TYPES(op_logical_and, "&&", TokenType::OpLogicalAnd)
LEXER_TYPES(op_logical_or, "||", TokenType::OpLogicalOr)
LEXER_TYPES(op_amp, "&", TokenType::OpAmp)
LEXER_TYPES(op_pipe, "|", TokenType::OpPipe)
LEXER_TYPES(op_caret, "^", TokenType::OpCaret)
LEXER_TYPES(op_tilde, "~", TokenType::OpTilde)
LEXER_TYPES(op_less_less, "<<", TokenType::OpLessLess)
LEXER_TYPES(op_greater_greater, ">>", TokenType::OpGreaterGreater)
LEXER_TYPES(op_plus_plus, "++", TokenType::OpPlusPlus)
LEXER_TYPES(op_minus_minus, "--", TokenType::OpMinusMinus)
LEXER_TYPES(op_arrow, "->", TokenType::OpArrow)
LEXER_TYPES(op_fat_arrow, "=>", TokenType::OpFatArrow)
LEXER_TYPES(op_colon_colon, "::", TokenType::OpColonColon)
LEXER_TYPES(op_dot, ".", TokenType::OpDot)
LEXER_TYPES(op_dot_dot, "..", TokenType::OpDotDot)
LEXER_TYPES(op_dot_dot_dot, "...", TokenType::OpDotDotDot)
LEXER_TYPES(op_question, "?", TokenType::OpQuestion)
LEXER_TYPES(op_question_question, "??", TokenType::OpQuestionQuestion)
LEXER_TYPES(op_at, "@", TokenType::OpAt)
LEXER_TYPES(op_hash, "#", TokenType::OpHash)
LEXER_TYPES(op_dollar, "$", TokenType::OpDollar)
LEXER_TYPES(op_plus_assign, "+=", TokenType::OpPlusAssign)
LEXER_TYPES(op_minus_assign, "-=", TokenType::OpMinusAssign)
LEXER_TYPES(op_star_assign, "*=", TokenType::OpStarAssign)
LEXER_TYPES(op_slash_assign, "/=", TokenType::OpSlashAssign)
LEXER_TYPES(op_percent_assign, "%=", TokenType::OpPercentAssign)
LEXER_TYPES(op_longest_greater_equal, ">=", TokenType::OpGreaterEqual)
LEXER_LEXEME(op_longest_ge_lexeme, "a>=b", 1, ">=", TokenType::OpGreaterEqual)
LEXER_TYPES(op_not_equal_vs_bang, "!= =", TokenType::OpNotEqual, TokenType::OpAssign)
LEXER_TYPES(op_dot_dot_vs_dot, ".. .", TokenType::OpDotDot, TokenType::OpDot)
LEXER_TYPES(op_arrow_vs_minus, "-> -", TokenType::OpArrow, TokenType::OpMinus)
LEXER_TYPES(op_colon_vs_colon_colon, ": ::", TokenType::PunctColon, TokenType::OpColonColon)
LEXER_TYPES(op_chain, "a+b*c", TokenType::Identifier, TokenType::OpPlus, TokenType::Identifier,
            TokenType::OpStar, TokenType::Identifier)

// --- Delimiters & punctuation (20) ---
LEXER_TYPES(delim_parens, "()", TokenType::DelimLeftParen, TokenType::DelimRightParen)
LEXER_TYPES(delim_braces, "{}", TokenType::DelimLeftBrace, TokenType::DelimRightBrace)
LEXER_TYPES(delim_brackets, "[]", TokenType::DelimLeftBracket, TokenType::DelimRightBracket)
LEXER_TYPES(punct_comma, ",", TokenType::PunctComma)
LEXER_TYPES(punct_semi, ";", TokenType::PunctSemicolon)
LEXER_TYPES(punct_colon, ":", TokenType::PunctColon)
LEXER_TYPES(punct_bang, "!", TokenType::PunctBang)
LEXER_TYPES(delim_nested, "([{}])", TokenType::DelimLeftParen, TokenType::DelimLeftBracket,
            TokenType::DelimLeftBrace, TokenType::DelimRightBrace, TokenType::DelimRightBracket,
            TokenType::DelimRightParen)
LEXER_TYPES(punct_fn_sig, "fn f():", TokenType::KwFn, TokenType::Identifier, TokenType::DelimLeftParen,
            TokenType::DelimRightParen, TokenType::PunctColon)
LEXER_TYPES(punct_list, "a,b,c;", TokenType::Identifier, TokenType::PunctComma, TokenType::Identifier,
            TokenType::PunctComma, TokenType::Identifier, TokenType::PunctSemicolon)
LEXER_TYPES(punct_block, "{;}", TokenType::DelimLeftBrace, TokenType::PunctSemicolon,
            TokenType::DelimRightBrace)
LEXER_TYPES(punct_array, "[1,2]", TokenType::DelimLeftBracket, TokenType::IntegerLiteral,
            TokenType::PunctComma, TokenType::IntegerLiteral, TokenType::DelimRightBracket)
LEXER_TYPES(punct_call, "f(a)", TokenType::Identifier, TokenType::DelimLeftParen,
            TokenType::Identifier, TokenType::DelimRightParen)
LEXER_TYPES(punct_generics_like, "Box<int>", TokenType::Identifier, TokenType::OpLess,
            TokenType::TyInt, TokenType::OpGreater)
LEXER_TYPES(punct_not_equal_vs_bang_eq, "!==", TokenType::OpNotEqual, TokenType::OpAssign)
LEXER_TYPES(punct_question_colon, "?:", TokenType::OpQuestion, TokenType::PunctColon)
LEXER_TYPES(punct_at_hash, "@#", TokenType::OpAt, TokenType::OpHash)
LEXER_TYPES(punct_mixed, "(,);", TokenType::DelimLeftParen, TokenType::PunctComma,
            TokenType::DelimRightParen, TokenType::PunctSemicolon)
LEXER_TYPES(punct_dot_field, "obj.field", TokenType::Identifier, TokenType::OpDot,
            TokenType::Identifier)
LEXER_TYPES(punct_semi_repeat, ";;;", TokenType::PunctSemicolon, TokenType::PunctSemicolon,
            TokenType::PunctSemicolon)

// --- Directives & annotations (10) ---
MYC_TEST(lexer_prod_directive_include) {
    LexerOptions options;
    options.enable_directives = true;
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("#include <stdio.h>", options);
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::DirectiveInclude));
}

MYC_TEST(lexer_prod_directive_define) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("#define FOO 1");
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::DirectiveDefine));
}

MYC_TEST(lexer_prod_directive_if_chain) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("#if\n#else\n#endif\nlet");
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::DirectiveIf));
    MYC_ASSERT(result.tokens.Peek(1).Is(TokenType::DirectiveElse));
    MYC_ASSERT(result.tokens.Peek(2).Is(TokenType::DirectiveEndif));
    MYC_ASSERT(result.tokens.Peek(3).Is(TokenType::KwLet));
}

MYC_TEST(lexer_prod_directive_pragma) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("#pragma once");
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::DirectivePragma));
}

MYC_TEST(lexer_prod_directive_disabled) {
    LexerOptions options;
    options.enable_directives = false;
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("#", options);
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::OpHash));
}

MYC_TEST(lexer_prod_annotation_simple) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("@deprecated");
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::Annotation));
}

MYC_TEST(lexer_prod_annotation_before_fn) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("@inline fn f()");
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::Annotation));
    MYC_ASSERT(result.tokens.Peek(1).Is(TokenType::KwFn));
}

MYC_TEST(lexer_prod_annotation_disabled) {
    LexerOptions options;
    options.enable_annotations = false;
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("@x", options);
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::OpAt));
    MYC_ASSERT(result.tokens.Peek(1).Is(TokenType::Identifier));
}

MYC_TEST(lexer_prod_hash_not_directive) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("x # y");
    ExpectNoErrors(result);
    ExpectTypes(result, {TokenType::Identifier, TokenType::OpHash, TokenType::Identifier});
}

MYC_TEST(lexer_prod_annotation_with_underscore) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("@my_attr");
    ExpectNoErrors(result);
    ExpectLexeme(result, 0, "@my_attr");
}

// --- Error recovery (15) ---
LEXER_TYPES_ERR(err_unknown_single, "\x01", 1, TokenType::Error)
LEXER_TYPES_ERR(err_unknown_two, "\x01\x02", 2, TokenType::Error, TokenType::Error)
MYC_TEST(lexer_prod_err_recovery_continues_parsing) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("\x01let");
    ExpectErrors(result, 1);
    MYC_ASSERT(result.tokens.Peek(1).Is(TokenType::KwLet));
}

MYC_TEST(lexer_prod_err_recovery_after_bad_string) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("\"bad\nok");
    ExpectErrors(result, 1);
    MYC_ASSERT(result.tokens.Peek(1).Is(TokenType::Identifier));
}

MYC_TEST(lexer_prod_err_recovery_after_bad_char) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("'bad");
    ExpectErrors(result, 1);
}

MYC_TEST(lexer_prod_err_recovery_after_bad_number) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("0x let");
    ExpectErrors(result, 1);
    MYC_ASSERT(result.tokens.Peek(1).Is(TokenType::KwLet));
}

MYC_TEST(lexer_prod_err_recovery_mixed) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("1 \x02 + 3");
    ExpectErrors(result, 1);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::IntegerLiteral));
    MYC_ASSERT(result.tokens.Peek(2).Is(TokenType::OpPlus));
}

MYC_TEST(lexer_prod_err_token_flags) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("\x05");
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::Error));
    MYC_ASSERT(result.tokens.Peek(0).HasFlag(myc::lexer::TokenFlags::Recoverable));
}

MYC_TEST(lexer_prod_err_multiple_strings) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("\"a\n\"b\"");
    MYC_ASSERT(result.had_errors);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::Error));
    bool found_string = false;
    for (std::size_t i = 0; i < result.tokens.Size(); ++i) {
        if (result.tokens.Peek(i).Is(TokenType::StringLiteral)) {
            found_string = true;
            break;
        }
    }
    MYC_ASSERT(found_string);
}

MYC_TEST(lexer_prod_err_invalid_escape_then_code) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("\"\\q\" 1");
    ExpectErrors(result, 1);
    MYC_ASSERT(result.tokens.Peek(2).Is(TokenType::IntegerLiteral));
}

MYC_TEST(lexer_prod_err_unterminated_block_then_code) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("/* unterminated\nlet");
    ExpectErrors(result, 1);
}

MYC_TEST(lexer_prod_err_stop_on_error_disabled) {
    LexerOptions options;
    options.continue_after_errors = true;
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("\x01\x02\x03", options);
    ExpectErrors(result, 3);
    MYC_ASSERT(result.tokens.Size() == 4u);
}

MYC_TEST(lexer_prod_err_recovery_preserves_eof) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("\x01");
    ExpectErrors(result, 1);
    MYC_ASSERT(result.tokens.Peek(1).IsEOF());
}

MYC_TEST(lexer_prod_err_bad_number_suffix) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("123xyz");
    ExpectErrors(result, 1);
}

MYC_TEST(lexer_prod_err_control_chars) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("\x1f\x7f");
    ExpectErrors(result, 2);
}

// --- EOF & integration (10) ---
MYC_TEST(lexer_prod_eof_empty_source) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("");
    ExpectNoErrors(result);
    ExpectTypes(result, {});
}

MYC_TEST(lexer_prod_eof_after_tokens) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("let x = 1");
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(result.tokens.Size() - 1).IsEOF());
}

MYC_TEST(lexer_prod_eof_only_whitespace) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize(" \n\t ");
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).IsEOF());
}

MYC_TEST(lexer_prod_full_function) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("fn add(a: int, b: int): int { return a + b; }");
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Size() > 10u);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::KwFn));
}

MYC_TEST(lexer_prod_match_expression) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("match x { 1 => 2, _ => 0 }");
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::KwMatch));
}

MYC_TEST(lexer_prod_import_statement) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("import std::io;");
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::KwImport));
    MYC_ASSERT(result.tokens.Peek(2).Is(TokenType::OpColonColon));
}

MYC_TEST(lexer_prod_struct_def) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("struct Point { x: float, y: float }");
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::KwStruct));
}

MYC_TEST(lexer_prod_async_await) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("async fn f() { await g(); }");
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).Is(TokenType::KwAsync));
    MYC_ASSERT(result.tokens.Peek(6).Is(TokenType::KwAwait));
}

MYC_TEST(lexer_prod_statistics_enabled) {
    LexerOptions options;
    options.collect_statistics = true;
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("let x = 1;", options);
    ExpectNoErrors(result);
    MYC_ASSERT(result.statistics.GetTokensProduced() >= 5u);
    MYC_ASSERT(result.statistics.GetScannerCalls() > 0u);
    MYC_ASSERT(result.statistics.GetElapsedTime().count() >= 0);
}

MYC_TEST(lexer_prod_token_lengths) {
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize("hello");
    MYC_ASSERT_EQ(result.tokens.Peek(0).GetLength(), 5u);
}

// --- Large files & performance (5) ---
MYC_TEST(lexer_prod_large_identifiers) {
    std::string source;
    source.reserve(10000);
    for (int i = 0; i < 1000; ++i) {
        source += "id";
        source += std::to_string(i);
        source += ' ';
    }
    LexerOptions options;
    options.collect_statistics = true;
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize(source, options);
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Size() > 1000u);
}

MYC_TEST(lexer_prod_large_numbers) {
    std::string source;
    for (int i = 0; i < 500; ++i) {
        source += std::to_string(i);
        source += ' ';
    }
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize(source);
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Size() > 500u);
}

MYC_TEST(lexer_prod_large_operators) {
    std::string source(2000, ';');
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize(source);
    ExpectNoErrors(result);
    MYC_ASSERT_EQ(result.tokens.Size(), 2001u);
}

MYC_TEST(lexer_prod_performance_many_tokens) {
    std::string source = "let x = ";
    for (int i = 0; i < 200; ++i) {
        source += std::to_string(i);
        source += " + ";
    }
    source += "0;";
    LexerOptions options;
    options.collect_statistics = true;
    const LexerHarness harness;
    const auto start = std::chrono::steady_clock::now();
    const LexerResult result = harness.Tokenize(source, options);
    const auto end = std::chrono::steady_clock::now();
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Size() > 400u);
    MYC_ASSERT(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() < 5000);
}

MYC_TEST(lexer_prod_streaming_large_line) {
    std::string source(100000, 'a');
    source += " end";
    const LexerHarness harness;
    const LexerResult result = harness.Tokenize(source);
    ExpectNoErrors(result);
    MYC_ASSERT(result.tokens.Peek(0).GetLexeme().size() == 100000u);
}

// --- Edge cases (15) ---
LEXER_TYPES(edge_not_not_equal, "!!", TokenType::PunctBang, TokenType::PunctBang)
LEXER_TYPES(edge_triple_equals, "===", TokenType::OpEqual, TokenType::OpAssign)
LEXER_TYPES(edge_dot_number, "...0", TokenType::OpDotDotDot, TokenType::IntegerLiteral)
LEXER_TYPES(edge_arrow_assign, "->=", TokenType::OpArrow, TokenType::OpAssign)
LEXER_TYPES(edge_angle_ops, "<<=>>", TokenType::OpLessLessAssign, TokenType::OpGreaterGreater)
LEXER_TYPES(edge_zero_x_identifier, "0xffff", TokenType::IntegerLiteral)
LEXER_TYPES(edge_string_with_comment, "\"/*\"", TokenType::StringLiteral)
LEXER_TYPES(edge_char_with_slash, "'/'", TokenType::CharacterLiteral)
LEXER_TYPES_ERR(edge_backslash_id, "\\x", 1, TokenType::Error, TokenType::Identifier)
LEXER_TYPES(edge_plus_dot, "+.", TokenType::OpPlus, TokenType::OpDot)
LEXER_TYPES(edge_negative_arrow, "-->", TokenType::OpMinusMinus, TokenType::OpGreater)
LEXER_TYPES(edge_question_assign, "?=", TokenType::OpQuestion, TokenType::OpAssign)
LEXER_TYPES(edge_colon_ident, ":foo", TokenType::PunctColon, TokenType::Identifier)
LEXER_TYPES(edge_self_type, "Self", TokenType::KwSelfType)
LEXER_TYPES(edge_mod_package, "mod package", TokenType::KwMod, TokenType::KwPackage)

#undef LEXER_TYPES
#undef LEXER_TYPES_ERR
#undef LEXER_LEXEME
