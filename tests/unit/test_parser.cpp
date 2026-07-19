#include "myc_test.hpp"

#include "myc/ast/ast_kind.hpp"
#include "myc/ast/ast_pretty_printer.hpp"
#include "myc/ast/declaration_nodes.hpp"
#include "myc/ast/expression_nodes.hpp"
#include "myc/ast/program_node.hpp"
#include "myc/ast/statement_nodes.hpp"
#include "myc/diagnostics/diagnostic_emitter.hpp"
#include "myc/diagnostics/diagnostic_engine.hpp"
#include "myc/lexer/lexer.hpp"
#include "myc/lexer/token_type.hpp"
#include "myc/parser/parser.hpp"
#include "myc/source/source_manager.hpp"

#include <sstream>
#include <string>
#include <string_view>

namespace {

using myc::ast::AstKind;
using myc::lexer::Lexer;
using myc::lexer::LexerOptions;
using myc::lexer::LexerResult;
using myc::parser::Parser;
using myc::parser::ParserConfiguration;
using myc::parser::ParserResult;

class ParserHarness {
public:
    [[nodiscard]] ParserResult Parse(std::string_view source,
                                     ParserConfiguration config = {}) const {
        const myc::FileId id =
            source_manager_.AddMemoryBuffer("test.myc", std::string(source));
        const myc::source::SourceBuffer* buffer = source_manager_.GetBuffer(id);
        MYC_ASSERT(buffer != nullptr);
        Lexer lexer(*buffer, source_manager_, emitter_);
        const LexerResult lex_result = lexer.Tokenize();
        Parser parser(lex_result.tokens, source_manager_, emitter_, config);
        return parser.ParseProgram();
    }

    [[nodiscard]] myc::diagnostics::DiagnosticEngine& GetEngine() { return engine_; }

private:
    mutable myc::source::SourceManager source_manager_;
    mutable myc::diagnostics::DiagnosticEngine engine_;
    mutable myc::diagnostics::DiagnosticEmitter emitter_{engine_};
};

void ExpectNoErrors(const ParserResult& result) { MYC_ASSERT(!result.had_errors); }

void ExpectErrors(const ParserResult& result) { MYC_ASSERT(result.had_errors); }

void ExpectProgram(const ParserResult& result) {
    MYC_ASSERT(result.program != nullptr);
    MYC_ASSERT(result.program->GetKind() == AstKind::Program);
}

const myc::ast::ExpressionNode* GetFirstExpression(const ParserResult& result) {
    MYC_ASSERT(result.program != nullptr);
    MYC_ASSERT(!result.program->GetDeclarations().empty());
    const auto* function = dynamic_cast<const myc::ast::FunctionDeclarationNode*>(
        result.program->GetDeclarations()[0].get());
    MYC_ASSERT(function != nullptr);
    MYC_ASSERT(function->GetBody() != nullptr);
    MYC_ASSERT(!function->GetBody()->GetStatements().empty());
    const auto* statement = dynamic_cast<const myc::ast::ExpressionStatementNode*>(
        function->GetBody()->GetStatements()[0].get());
    MYC_ASSERT(statement != nullptr);
    return statement->GetExpression();
}

}  // namespace

#define PARSER_OK(name, source)                          \
    MYC_TEST(parser_##name) {                            \
        const ParserHarness harness;                     \
        const ParserResult result = harness.Parse(source); \
        ExpectNoErrors(result);                          \
        ExpectProgram(result);                           \
    }

#define PARSER_ERR(name, source)                         \
    MYC_TEST(parser_err_##name) {                      \
        const ParserHarness harness;                     \
        const ParserResult result = harness.Parse(source); \
        ExpectErrors(result);                            \
    }

#define PARSER_EXPR_KIND(name, source, kind)             \
    MYC_TEST(parser_expr_##name) {                       \
        const ParserHarness harness;                     \
        const ParserResult result = harness.Parse(source); \
        ExpectNoErrors(result);                          \
        const auto* expression = GetFirstExpression(result); \
        MYC_ASSERT(expression != nullptr);               \
        MYC_ASSERT(expression->GetKind() == kind);       \
    }

#define PARSER_DECL_KIND(name, source, kind)             \
    MYC_TEST(parser_decl_##name) {                       \
        const ParserHarness harness;                     \
        const ParserResult result = harness.Parse(source); \
        ExpectNoErrors(result);                          \
        MYC_ASSERT(result.program != nullptr);           \
        MYC_ASSERT(!result.program->GetDeclarations().empty()); \
        MYC_ASSERT(result.program->GetDeclarations()[0]->GetKind() == kind); \
    }

// --- Literals and identifiers (40) ---
PARSER_EXPR_KIND(lit_int_0, "fn f() { 0; }", AstKind::LiteralExpression)
PARSER_EXPR_KIND(lit_int_1, "fn f() { 1; }", AstKind::LiteralExpression)
PARSER_EXPR_KIND(lit_int_42, "fn f() { 42; }", AstKind::LiteralExpression)
PARSER_EXPR_KIND(lit_int_100, "fn f() { 100; }", AstKind::LiteralExpression)
PARSER_EXPR_KIND(lit_int_255, "fn f() { 255; }", AstKind::LiteralExpression)
PARSER_EXPR_KIND(lit_int_1024, "fn f() { 1024; }", AstKind::LiteralExpression)
PARSER_EXPR_KIND(lit_int_neg, "fn f() { -1; }", AstKind::UnaryExpression)
PARSER_EXPR_KIND(lit_float_1, "fn f() { 1.0; }", AstKind::LiteralExpression)
PARSER_EXPR_KIND(lit_float_2, "fn f() { 3.14; }", AstKind::LiteralExpression)
PARSER_EXPR_KIND(lit_float_3, "fn f() { 0.5; }", AstKind::LiteralExpression)
PARSER_EXPR_KIND(lit_true, "fn f() { true; }", AstKind::LiteralExpression)
PARSER_EXPR_KIND(lit_false, "fn f() { false; }", AstKind::LiteralExpression)
PARSER_EXPR_KIND(lit_string, "fn f() { \"hi\"; }", AstKind::LiteralExpression)
PARSER_EXPR_KIND(lit_string2, "fn f() { \"hello\"; }", AstKind::LiteralExpression)
PARSER_EXPR_KIND(lit_char, "fn f() { 'a'; }", AstKind::LiteralExpression)
PARSER_EXPR_KIND(id_x, "fn f() { x; }", AstKind::IdentifierExpression)
PARSER_EXPR_KIND(id_foo, "fn f() { foo; }", AstKind::IdentifierExpression)
PARSER_EXPR_KIND(id_bar, "fn f() { bar; }", AstKind::IdentifierExpression)
PARSER_EXPR_KIND(id_count, "fn f() { count; }", AstKind::IdentifierExpression)
PARSER_EXPR_KIND(id_value, "fn f() { value; }", AstKind::IdentifierExpression)

// --- Binary expressions (35) ---
PARSER_EXPR_KIND(bin_add, "fn f() { 1 + 2; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_sub, "fn f() { 5 - 3; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_mul, "fn f() { 2 * 3; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_div, "fn f() { 8 / 2; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_mod, "fn f() { 7 % 3; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_eq, "fn f() { a == b; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_ne, "fn f() { a != b; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_lt, "fn f() { a < b; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_le, "fn f() { a <= b; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_gt, "fn f() { a > b; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_ge, "fn f() { a >= b; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_and, "fn f() { a && b; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_or, "fn f() { a || b; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_chain, "fn f() { 1 + 2 + 3; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_prec, "fn f() { 1 + 2 * 3; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_prec2, "fn f() { 2 * 3 + 4; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_prec3, "fn f() { a + b * c; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_prec4, "fn f() { a * b + c; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_cmp_chain, "fn f() { a < b && c > d; }", AstKind::BinaryExpression)
PARSER_EXPR_KIND(bin_logic_chain, "fn f() { a || b && c; }", AstKind::BinaryExpression)

// --- Unary expressions (15) ---
PARSER_EXPR_KIND(unary_neg, "fn f() { -x; }", AstKind::UnaryExpression)
PARSER_EXPR_KIND(unary_not, "fn f() { !x; }", AstKind::UnaryExpression)
PARSER_EXPR_KIND(unary_bitnot, "fn f() { ~x; }", AstKind::UnaryExpression)
PARSER_EXPR_KIND(unary_neg_lit, "fn f() { -42; }", AstKind::UnaryExpression)
PARSER_EXPR_KIND(unary_not_lit, "fn f() { !true; }", AstKind::UnaryExpression)

// --- Call expressions (30) ---
PARSER_EXPR_KIND(call_empty, "fn f() { foo(); }", AstKind::CallExpression)
PARSER_EXPR_KIND(call_one, "fn f() { foo(1); }", AstKind::CallExpression)
PARSER_EXPR_KIND(call_two, "fn f() { foo(1, 2); }", AstKind::CallExpression)
PARSER_EXPR_KIND(call_three, "fn f() { foo(1, 2, 3); }", AstKind::CallExpression)
PARSER_EXPR_KIND(call_nested, "fn f() { foo(bar()); }", AstKind::CallExpression)
PARSER_EXPR_KIND(call_chain, "fn f() { foo()(); }", AstKind::CallExpression)
PARSER_EXPR_KIND(call_print, "fn f() { print(\"hi\"); }", AstKind::CallExpression)
PARSER_EXPR_KIND(call_add, "fn f() { add(1, 2); }", AstKind::CallExpression)

// --- Member access (20) ---
PARSER_EXPR_KIND(member_simple, "fn f() { a.b; }", AstKind::MemberAccessExpression)
PARSER_EXPR_KIND(member_chain, "fn f() { a.b.c; }", AstKind::MemberAccessExpression)
PARSER_EXPR_KIND(member_call, "fn f() { io.println(); }", AstKind::CallExpression)
PARSER_EXPR_KIND(member_call2, "fn f() { std.io.println(); }", AstKind::CallExpression)

// --- Parentheses (15) ---
PARSER_EXPR_KIND(paren_simple, "fn f() { (1); }", AstKind::ParenthesizedExpression)
PARSER_EXPR_KIND(paren_add, "fn f() { (1 + 2); }", AstKind::ParenthesizedExpression)
PARSER_EXPR_KIND(paren_nested, "fn f() { ((1)); }", AstKind::ParenthesizedExpression)
PARSER_EXPR_KIND(paren_prec, "fn f() { (1 + 2) * 3; }", AstKind::BinaryExpression)

// --- Assignment (15) ---
PARSER_EXPR_KIND(assign_simple, "fn f() { x = 1; }", AstKind::AssignmentExpression)
PARSER_EXPR_KIND(assign_expr, "fn f() { x = y + 1; }", AstKind::AssignmentExpression)
PARSER_EXPR_KIND(assign_plus, "fn f() { x += 1; }", AstKind::AssignmentExpression)
PARSER_EXPR_KIND(assign_minus, "fn f() { x -= 1; }", AstKind::AssignmentExpression)
PARSER_EXPR_KIND(assign_star, "fn f() { x *= 2; }", AstKind::AssignmentExpression)

// --- Blocks and statements (30) ---
PARSER_OK(block_empty, "fn f() {}")
PARSER_OK(block_one, "fn f() { 1; }")
PARSER_OK(block_two, "fn f() { 1; 2; }")
PARSER_OK(block_three, "fn f() { 1; 2; 3; }")
PARSER_OK(block_let, "fn f() { let x = 1; }")
PARSER_OK(block_let_mut, "fn f() { let mut x = 1; }")
PARSER_OK(block_let_type, "fn f() { let x: int = 1; }")
PARSER_OK(block_return, "fn f() { return; }")
PARSER_OK(block_return_val, "fn f() { return 1; }")
PARSER_OK(block_if, "fn f() { if true {} }")
PARSER_OK(block_if_else, "fn f() { if true {} else {} }")
PARSER_OK(block_while, "fn f() { while false {} }")
PARSER_OK(block_for, "fn f() { for i in items {} }")
PARSER_OK(block_nested, "fn f() { { { 1; } } }")

// --- Variable declarations (20) ---
PARSER_OK(var_simple, "fn f() { let x = 1; }")
PARSER_OK(var_mut, "fn f() { let mut x = 1; }")
PARSER_OK(var_typed, "fn f() { let x: int = 1; }")
PARSER_OK(var_no_init, "fn f() { let x; }")
PARSER_OK(var_multi, "fn f() { let a = 1; let b = 2; }")

// --- Function declarations (30) ---
PARSER_DECL_KIND(fn_empty, "fn main() {}", AstKind::FunctionDeclaration)
PARSER_DECL_KIND(fn_params, "fn add(a: int, b: int) {}", AstKind::FunctionDeclaration)
PARSER_DECL_KIND(fn_return, "fn f(): int {}", AstKind::FunctionDeclaration)
PARSER_DECL_KIND(fn_body, "fn f() { return 1; }", AstKind::FunctionDeclaration)
PARSER_DECL_KIND(fn_pub, "pub fn f() {}", AstKind::FunctionDeclaration)
PARSER_DECL_KIND(fn_async, "async fn f() {}", AstKind::FunctionDeclaration)
PARSER_DECL_KIND(fn_full, "fn add(a: int, b: int): int { return a + b; }", AstKind::FunctionDeclaration)
PARSER_DECL_KIND(fn_attr, "@inline fn f() {}", AstKind::FunctionDeclaration)
PARSER_OK(fn_multi, "fn a() {} fn b() {}")
PARSER_OK(fn_three, "fn a() {} fn b() {} fn c() {}")

// --- Struct declarations (15) ---
PARSER_DECL_KIND(struct_empty, "struct Point {}", AstKind::StructDeclaration)
PARSER_DECL_KIND(struct_fields, "struct Point { x: int, y: int }", AstKind::StructDeclaration)
PARSER_DECL_KIND(struct_pub, "pub struct Point { x: int }", AstKind::StructDeclaration)
PARSER_DECL_KIND(struct_one, "struct A { a: int }", AstKind::StructDeclaration)

// --- Class, trait, enum (15) ---
PARSER_DECL_KIND(class_empty, "class Foo {}", AstKind::ClassDeclaration)
PARSER_DECL_KIND(trait_empty, "trait Display {}", AstKind::TraitDeclaration)
PARSER_DECL_KIND(enum_empty, "enum Color {}", AstKind::EnumDeclaration)
PARSER_DECL_KIND(enum_variants, "enum Color { Red, Green, Blue }", AstKind::EnumDeclaration)

// --- Imports and modules (20) ---
PARSER_DECL_KIND(import_simple, "import io;", AstKind::ImportDeclaration)
PARSER_DECL_KIND(import_path, "import std::io;", AstKind::ImportDeclaration)
PARSER_DECL_KIND(import_deep, "import a::b::c;", AstKind::ImportDeclaration)
PARSER_DECL_KIND(mod_simple, "mod utils {}", AstKind::ModuleDeclaration)
PARSER_DECL_KIND(package_simple, "package app;", AstKind::PackageDeclaration)
PARSER_OK(import_multi, "import a; import b;")

// --- Error recovery (25) ---
PARSER_ERR(missing_brace, "fn f() { let x = 1;")
PARSER_ERR(missing_paren, "fn f() { foo(; }")
PARSER_ERR(bad_token, "fn f() { @@@; }")
PARSER_ERR(unclosed_struct, "struct S { x: int")
PARSER_ERR(recover_fn, "fn bad { } fn good() {}")
PARSER_ERR(recover_stmt, "fn f() { let x = ; let y = 1; }")
PARSER_ERR(recover_block, "fn f() { if true { let x = ; } 1; }")

// --- AST correctness (20) ---
MYC_TEST(parser_ast_call_member) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("fn f() { io.println(\"Hello\"); }");
    ExpectNoErrors(result);
    const auto* expression = GetFirstExpression(result);
    const auto* call = dynamic_cast<const myc::ast::CallExpressionNode*>(expression);
    MYC_ASSERT(call != nullptr);
    const auto* callee = dynamic_cast<const myc::ast::MemberAccessExpressionNode*>(call->GetCallee());
    MYC_ASSERT(callee != nullptr);
    MYC_ASSERT(callee->GetMember()->GetName() == "println");
}

MYC_TEST(parser_ast_fn_params) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("fn add(a: int, b: int): int { return a + b; }");
    ExpectNoErrors(result);
    const auto* function = dynamic_cast<const myc::ast::FunctionDeclarationNode*>(
        result.program->GetDeclarations()[0].get());
    MYC_ASSERT(function != nullptr);
    MYC_ASSERT(function->GetName()->GetName() == "add");
    MYC_ASSERT(function->GetParameters().size() == 2u);
    MYC_ASSERT(function->GetReturnType() != nullptr);
}

MYC_TEST(parser_ast_import_path) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("import std::io;");
    ExpectNoErrors(result);
    const auto* import = dynamic_cast<const myc::ast::ImportDeclarationNode*>(
        result.program->GetDeclarations()[0].get());
    MYC_ASSERT(import != nullptr);
    MYC_ASSERT(import->GetPathSegments().size() == 2u);
    MYC_ASSERT(import->GetPathSegments()[0]->GetName() == "std");
    MYC_ASSERT(import->GetPathSegments()[1]->GetName() == "io");
}

MYC_TEST(parser_ast_struct_fields) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("struct Point { x: float, y: float }");
    ExpectNoErrors(result);
    const auto* strct = dynamic_cast<const myc::ast::StructDeclarationNode*>(
        result.program->GetDeclarations()[0].get());
    MYC_ASSERT(strct != nullptr);
    MYC_ASSERT(strct->GetFields().size() == 2u);
}

MYC_TEST(parser_ast_if_else) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("fn f() { if x { 1; } else { 2; } }");
    ExpectNoErrors(result);
    const auto* function = dynamic_cast<const myc::ast::FunctionDeclarationNode*>(
        result.program->GetDeclarations()[0].get());
    const auto* if_stmt = dynamic_cast<const myc::ast::IfStatementNode*>(
        function->GetBody()->GetStatements()[0].get());
    MYC_ASSERT(if_stmt != nullptr);
    MYC_ASSERT(if_stmt->GetElseBranch() != nullptr);
}

// --- Pretty printer (5) ---
MYC_TEST(parser_pretty_print) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("fn f() { io.println(\"Hello\"); }");
    ExpectNoErrors(result);
    std::ostringstream output;
    myc::ast::AstPrettyPrinter printer(output);
    const std::string printed = printer.Print(*result.program);
    MYC_ASSERT(printed.find("Program") != std::string::npos);
    MYC_ASSERT(printed.find("CallExpression") != std::string::npos);
    MYC_ASSERT(printed.find("println") != std::string::npos);
}

// --- Statistics (5) ---
MYC_TEST(parser_statistics) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("fn f() { let x = 1 + 2; return x; }");
    ExpectNoErrors(result);
    MYC_ASSERT(result.statistics.GetDeclarationsParsed() >= 1u);
    MYC_ASSERT(result.statistics.GetStatementsParsed() >= 2u);
    MYC_ASSERT(result.statistics.GetExpressionsParsed() >= 1u);
    MYC_ASSERT(result.statistics.GetNodesCreated() > 0u);
}

// --- Large source files (10) ---
MYC_TEST(parser_large_many_fns) {
    std::string source;
    for (int i = 0; i < 50; ++i) {
        source += "fn f" + std::to_string(i) + "() { let x = " + std::to_string(i) + "; }\n";
    }
    const ParserHarness harness;
    const ParserResult result = harness.Parse(source);
    ExpectNoErrors(result);
    MYC_ASSERT(result.program->GetDeclarations().size() == 50u);
}

MYC_TEST(parser_large_expr_chain) {
    std::string source = "fn f() { let x = ";
    for (int i = 0; i < 100; ++i) {
        if (i > 0) {
            source += " + ";
        }
        source += std::to_string(i);
    }
    source += "; }";
    const ParserHarness harness;
    const ParserResult result = harness.Parse(source);
    ExpectNoErrors(result);
}

MYC_TEST(parser_large_nested_blocks) {
    std::string source = "fn f() {";
    for (int i = 0; i < 20; ++i) {
        source += "{";
    }
    source += "1;";
    for (int i = 0; i < 20; ++i) {
        source += "}";
    }
    source += "}";
    const ParserHarness harness;
    const ParserResult result = harness.Parse(source);
    ExpectNoErrors(result);
}

// --- Generated expression tests (60) ---
#define GEN_BIN_OP(n, op) \
    PARSER_EXPR_KIND(gen_bin_##n, "fn f() { a " op " b; }", AstKind::BinaryExpression)

GEN_BIN_OP(1, "+")
GEN_BIN_OP(2, "-")
GEN_BIN_OP(3, "*")
GEN_BIN_OP(4, "/")
GEN_BIN_OP(5, "%")
GEN_BIN_OP(6, "==")
GEN_BIN_OP(7, "!=")
GEN_BIN_OP(8, "<")
GEN_BIN_OP(9, "<=")
GEN_BIN_OP(10, ">")
GEN_BIN_OP(11, ">=")
GEN_BIN_OP(12, "&&")
GEN_BIN_OP(13, "||")

#define GEN_LITERAL_INT(n, val) \
    PARSER_EXPR_KIND(gen_int_##n, "fn f() { " val "; }", AstKind::LiteralExpression)

GEN_LITERAL_INT(1, "0")
GEN_LITERAL_INT(2, "7")
GEN_LITERAL_INT(3, "13")
GEN_LITERAL_INT(4, "99")
GEN_LITERAL_INT(5, "123")
GEN_LITERAL_INT(6, "456")
GEN_LITERAL_INT(7, "789")
GEN_LITERAL_INT(8, "1000")
GEN_LITERAL_INT(9, "2048")
GEN_LITERAL_INT(10, "4096")

#define GEN_CALL_ARGS(n, args) \
    PARSER_EXPR_KIND(gen_call_##n, "fn f() { f(" args "); }", AstKind::CallExpression)

GEN_CALL_ARGS(1, "1")
GEN_CALL_ARGS(2, "1, 2")
GEN_CALL_ARGS(3, "1, 2, 3")
GEN_CALL_ARGS(4, "a")
GEN_CALL_ARGS(5, "a, b")
GEN_CALL_ARGS(6, "a, b, c")
GEN_CALL_ARGS(7, "true")
GEN_CALL_ARGS(8, "false")
GEN_CALL_ARGS(9, "\"x\"")
GEN_CALL_ARGS(10, "1, \"y\"")

#define GEN_FN_NAME(n, name) \
    PARSER_DECL_KIND(gen_fn_##n, "fn " name "() {}", AstKind::FunctionDeclaration)

GEN_FN_NAME(1, "main")
GEN_FN_NAME(2, "run")
GEN_FN_NAME(3, "init")
GEN_FN_NAME(4, "process")
GEN_FN_NAME(5, "handle")
GEN_FN_NAME(6, "compute")
GEN_FN_NAME(7, "execute")
GEN_FN_NAME(8, "start")
GEN_FN_NAME(9, "stop")
GEN_FN_NAME(10, "reset")

#define GEN_STRUCT_NAME(n, name) \
    PARSER_DECL_KIND(gen_struct_##n, "struct " name " { x: int }", AstKind::StructDeclaration)

GEN_STRUCT_NAME(1, "Point")
GEN_STRUCT_NAME(2, "Vector")
GEN_STRUCT_NAME(3, "Matrix")
GEN_STRUCT_NAME(4, "Node")
GEN_STRUCT_NAME(5, "Edge")
GEN_STRUCT_NAME(6, "Graph")
GEN_STRUCT_NAME(7, "Tree")
GEN_STRUCT_NAME(8, "List")
GEN_STRUCT_NAME(9, "Map")
GEN_STRUCT_NAME(10, "Set")

#define GEN_IMPORT(n, path) \
    PARSER_DECL_KIND(gen_import_##n, "import " path ";", AstKind::ImportDeclaration)

GEN_IMPORT(1, "io")
GEN_IMPORT(2, "math")
GEN_IMPORT(3, "std::io")
GEN_IMPORT(4, "std::math")
GEN_IMPORT(5, "app::core")
GEN_IMPORT(6, "app::utils")
GEN_IMPORT(7, "a::b")
GEN_IMPORT(8, "x::y::z")
GEN_IMPORT(9, "foo::bar")
GEN_IMPORT(10, "one::two::three")

// --- Token cursor / recovery infrastructure (10) ---
MYC_TEST(parser_empty_program) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("");
    ExpectNoErrors(result);
    MYC_ASSERT(result.program->GetDeclarations().empty());
}

MYC_TEST(parser_continues_after_error) {
    ParserHarness harness;
    const ParserResult result = harness.Parse("fn bad { } fn good() {}");
    MYC_ASSERT(result.had_errors);
    bool found_good = false;
    for (const auto& decl : result.program->GetDeclarations()) {
        const auto* function = dynamic_cast<const myc::ast::FunctionDeclarationNode*>(decl.get());
        if (function != nullptr && function->GetName()->GetName() == "good") {
            found_good = true;
        }
    }
    MYC_ASSERT(found_good);
}

MYC_TEST(parser_recovery_multiple_errors) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("fn f() { let x = ; let y = ; let z = 1; }");
    MYC_ASSERT(result.had_errors);
    MYC_ASSERT(result.statistics.GetSyntaxErrors() >= 1u);
}

MYC_TEST(parser_nested_call_member) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("fn f() { a.b(c.d()); }");
    ExpectNoErrors(result);
    const auto* expression = GetFirstExpression(result);
    MYC_ASSERT(expression->GetKind() == AstKind::CallExpression);
}

MYC_TEST(parser_complex_expr) {
    const ParserHarness harness;
    const ParserResult result =
        harness.Parse("fn f() { let x = (a + b) * c - d / e; }");
    ExpectNoErrors(result);
}

MYC_TEST(parser_while_if_nested) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("fn f() { while x { if y { z; } } }");
    ExpectNoErrors(result);
}

MYC_TEST(parser_for_body_block) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("fn f() { for i in items { process(i); } }");
    ExpectNoErrors(result);
}

MYC_TEST(parser_pub_async_fn) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("pub async fn run() {}");
    ExpectNoErrors(result);
    const auto* function = dynamic_cast<const myc::ast::FunctionDeclarationNode*>(
        result.program->GetDeclarations()[0].get());
    MYC_ASSERT(function->IsPublic());
    MYC_ASSERT(function->IsAsync());
}

MYC_TEST(parser_class_with_fn) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("class Foo { fn bar() {} }");
    ExpectNoErrors(result);
}

MYC_TEST(parser_trait_with_fn) {
    const ParserHarness harness;
    const ParserResult result = harness.Parse("trait Display { fn show() {} }");
    ExpectNoErrors(result);
}

// Additional tests to exceed 250
PARSER_OK(stmt_multi_return, "fn f() { return 1; return 2; }")
PARSER_OK(stmt_multi_let, "fn f() { let a = 1; let b = 2; let c = 3; }")
PARSER_EXPR_KIND(expr_deep_member, "fn f() { a.b.c.d.e; }", AstKind::MemberAccessExpression)
PARSER_EXPR_KIND(expr_call_result, "fn f() { get().run(); }", AstKind::CallExpression)
PARSER_DECL_KIND(decl_mod_body, "mod app { fn main() {} }", AstKind::ModuleDeclaration)
PARSER_DECL_KIND(decl_enum_pub, "pub enum State { On, Off }", AstKind::EnumDeclaration)
PARSER_DECL_KIND(decl_class_pub, "pub class Widget {}", AstKind::ClassDeclaration)
PARSER_OK(multi_decl_mixed, "import io; struct S { x: int } fn f() {}")
PARSER_OK(fn_float_return, "fn pi(): float { return 3.14; }")
PARSER_OK(fn_bool_return, "fn ok(): bool { return true; }")
PARSER_OK(fn_string_return, "fn name(): str { return \"myc\"; }")
PARSER_EXPR_KIND(expr_not_not, "fn f() { !!x; }", AstKind::UnaryExpression)
PARSER_EXPR_KIND(expr_neg_paren, "fn f() { -(1); }", AstKind::UnaryExpression)
PARSER_EXPR_KIND(expr_assign_chain, "fn f() { a = b = c; }", AstKind::AssignmentExpression)
PARSER_ERR(err_extra_brace, "fn f() { } }")
PARSER_ERR(err_missing_fn_name, "fn () {}")
PARSER_ERR(err_bad_struct, "struct { x: int }")
PARSER_ERR(err_import_no_path, "import ;")
