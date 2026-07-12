#include "myc_test.hpp"

#include "myc/lexer/keyword.hpp"
#include "myc/lexer/operator.hpp"
#include "myc/lexer/token_factory.hpp"
#include "myc/lexer/token_flags.hpp"
#include "myc/lexer/token_printer.hpp"
#include "myc/lexer/token_stream.hpp"
#include "myc/lexer/token_type.hpp"
#include "myc/source/source_file_id.hpp"
#include "myc/source/source_location.hpp"
#include "myc/source/source_span.hpp"

#include <string>
#include <vector>

namespace {

myc::source::SourceLocation MakeLocation(std::size_t offset, myc::LineNumber line = 1,
                                         myc::ColumnNumber column = 1) {
    return myc::source::SourceLocation(myc::source::SourceFileID(1), offset, line, column);
}

myc::source::SourceSpan MakeSpan(std::size_t start_offset, std::size_t end_offset,
                                 myc::LineNumber line = 1) {
    return myc::source::SourceSpan(
        MakeLocation(start_offset, line, static_cast<myc::ColumnNumber>(start_offset + 1)),
        MakeLocation(end_offset, line, static_cast<myc::ColumnNumber>(end_offset + 1)));
}

myc::lexer::Token MakeIdentifierToken(std::string_view lexeme, std::size_t offset) {
    return myc::lexer::TokenFactory::MakeIdentifier(lexeme, MakeLocation(offset),
                                                    MakeSpan(offset, offset + lexeme.size()));
}

myc::lexer::Token MakeKeywordToken(myc::lexer::TokenType type, std::string_view lexeme,
                                   std::size_t offset) {
    return myc::lexer::TokenFactory::MakeKeyword(type, lexeme, MakeLocation(offset),
                                                 MakeSpan(offset, offset + lexeme.size()));
}

myc::lexer::Token MakeIntegerToken(std::string_view lexeme, std::size_t offset) {
    return myc::lexer::TokenFactory::MakeLiteral(
        myc::lexer::TokenType::IntegerLiteral, myc::lexer::LiteralKind::Integer, lexeme,
        MakeLocation(offset), MakeSpan(offset, offset + lexeme.size()));
}

myc::lexer::Token MakeOperatorToken(std::string_view lexeme, std::size_t offset) {
    return myc::lexer::TokenFactory::MakeOperator(myc::lexer::TokenType::OpPlus, lexeme,
                                                  MakeLocation(offset),
                                                  MakeSpan(offset, offset + lexeme.size()));
}

myc::lexer::Token MakeEOFToken(std::size_t offset) {
    return myc::lexer::TokenFactory::MakeEOF(MakeLocation(offset), MakeSpan(offset, offset));
}

}  // namespace

MYC_TEST(token_factory_identifier_creation) {
    const auto token = MakeIdentifierToken("count", 0);
    MYC_ASSERT(token.GetType() == myc::lexer::TokenType::Identifier);
    MYC_ASSERT_EQ(token.GetLexeme(), "count");
    MYC_ASSERT(token.GetCategory() == myc::lexer::TokenCategory::Identifier);
    MYC_ASSERT_EQ(token.GetLength(), 5u);
    MYC_ASSERT(token.GetLocation().GetOffset() == 0u);
}

MYC_TEST(token_equality) {
    const auto left = MakeIdentifierToken("count", 0);
    const auto right = MakeIdentifierToken("count", 0);
    const auto different = MakeIdentifierToken("value", 6);
    MYC_ASSERT(left == right);
    MYC_ASSERT(!(left == different));
}

MYC_TEST(keyword_lookup) {
    const myc::lexer::Keyword* keyword = myc::lexer::KeywordTable::Lookup("if");
    MYC_ASSERT(keyword != nullptr);
    MYC_ASSERT(keyword->type == myc::lexer::TokenType::KwIf);
    MYC_ASSERT(keyword->IsReserved());

    const auto type = myc::lexer::KeywordTable::LookupType("struct");
    MYC_ASSERT(type.has_value());
    MYC_ASSERT(*type == myc::lexer::TokenType::KwStruct);

    MYC_ASSERT(!myc::lexer::KeywordTable::IsKeywordText("count"));
    MYC_ASSERT(myc::lexer::KeywordTable::Count() > 0u);
}

MYC_TEST(operator_lookup) {
    const myc::lexer::Operator* op = myc::lexer::OperatorTable::Lookup("+");
    MYC_ASSERT(op != nullptr);
    MYC_ASSERT(op->type == myc::lexer::TokenType::OpPlus);
    MYC_ASSERT(op->IsUnary());
    MYC_ASSERT(op->IsBinary());

    const myc::lexer::Operator* assign = myc::lexer::OperatorTable::Lookup("==");
    MYC_ASSERT(assign != nullptr);
    MYC_ASSERT(assign->precedence == 7);
    MYC_ASSERT(assign->associativity == myc::lexer::Associativity::Left);

    MYC_ASSERT(!myc::lexer::OperatorTable::IsOperatorSymbol("if"));
}

MYC_TEST(token_stream_navigation) {
    std::vector<myc::lexer::Token> tokens = {
        MakeIdentifierToken("count", 0),
        MakeOperatorToken("+", 6),
        MakeIntegerToken("42", 8),
        MakeEOFToken(10),
    };
    myc::lexer::TokenStream stream(std::move(tokens));

    MYC_ASSERT_EQ(stream.Size(), 4u);
    MYC_ASSERT(!stream.IsAtEnd());
    MYC_ASSERT_EQ(stream.Peek().GetLexeme(), "count");
    MYC_ASSERT_EQ(stream.Lookahead(1).GetLexeme(), "+");

    const myc::lexer::Token& first = stream.Consume();
    MYC_ASSERT_EQ(first.GetLexeme(), "count");
    MYC_ASSERT_EQ(stream.GetPosition(), 1u);
    MYC_ASSERT_EQ(stream.Previous().GetLexeme(), "count");

    const std::size_t checkpoint = stream.SaveCheckpoint();
    (void)stream.Consume();
    (void)stream.Consume();
    (void)stream.Consume();
    MYC_ASSERT(stream.IsAtEnd());
    stream.RestoreCheckpoint(checkpoint);
    MYC_ASSERT_EQ(stream.Peek().GetLexeme(), "+");

    stream.Reset();
    MYC_ASSERT_EQ(stream.GetPosition(), 0u);
    MYC_ASSERT_EQ(stream[2].GetLexeme(), "42");
}

MYC_TEST(token_stream_shared_parallel_cursors) {
    std::vector<myc::lexer::Token> tokens = {MakeIdentifierToken("a", 0),
                                             MakeIdentifierToken("b", 2)};
    myc::lexer::TokenStream primary(std::move(tokens));
    myc::lexer::TokenStream fork = primary.Share();

    (void)primary.Consume();
    MYC_ASSERT_EQ(primary.Peek().GetLexeme(), "b");
    MYC_ASSERT_EQ(fork.Peek().GetLexeme(), "a");
}

MYC_TEST(token_iterator_range_loop) {
    std::vector<myc::lexer::Token> tokens = {MakeIdentifierToken("x", 0),
                                             MakeKeywordToken(myc::lexer::TokenType::KwIf, "if", 2)};
    myc::lexer::TokenStream stream(std::move(tokens));

    std::size_t count = 0;
    for (const myc::lexer::Token& token : stream) {
        MYC_ASSERT(!token.GetLexeme().empty());
        ++count;
    }
    MYC_ASSERT_EQ(count, 2u);
}

MYC_TEST(token_categories_and_flags) {
    const auto keyword =
        MakeKeywordToken(myc::lexer::TokenType::KwIf, "if", 0);
    MYC_ASSERT(keyword.IsKeyword());
    MYC_ASSERT(keyword.GetCategory() == myc::lexer::TokenCategory::Keyword);

    const auto literal = MakeIntegerToken("0xFF", 4);
    MYC_ASSERT(literal.IsLiteral());
    MYC_ASSERT(literal.GetLiteralKind() == myc::lexer::LiteralKind::Integer);

    const auto flagged = myc::lexer::TokenFactory::MakeIdentifier(
        "x", MakeLocation(0), MakeSpan(0, 1),
        myc::lexer::TokenFlags::HasLeadingTrivia | myc::lexer::TokenFlags::AtStartOfLine);
    MYC_ASSERT(flagged.HasFlag(myc::lexer::TokenFlags::HasLeadingTrivia));
    MYC_ASSERT(flagged.HasFlag(myc::lexer::TokenFlags::AtStartOfLine));
}

MYC_TEST(token_printer_output) {
    const auto identifier = MakeIdentifierToken("count", 0);
    MYC_ASSERT_EQ(myc::lexer::TokenPrinter::Print(identifier), "Identifier(\"count\")");

    const auto integer = MakeIntegerToken("42", 0);
    MYC_ASSERT_EQ(myc::lexer::TokenPrinter::Print(integer), "IntegerLiteral(\"42\")");

    const auto keyword = MakeKeywordToken(myc::lexer::TokenType::KwIf, "if", 0);
    MYC_ASSERT_EQ(myc::lexer::TokenPrinter::Print(keyword), "Keyword(\"if\")");

    const auto op = MakeOperatorToken("+", 0);
    MYC_ASSERT_EQ(myc::lexer::TokenPrinter::Print(op), "Operator(\"+\")");

    const auto eof = MakeEOFToken(0);
    MYC_ASSERT_EQ(myc::lexer::TokenPrinter::Print(eof), "EOF");
}

MYC_TEST(token_eof_and_error_behavior) {
    myc::lexer::TokenStream empty;
    MYC_ASSERT(empty.IsAtEnd());
    MYC_ASSERT(empty.Peek().IsEOF());

    const auto error = myc::lexer::TokenFactory::MakeError("@", MakeLocation(0),
                                                            MakeSpan(0, 1));
    MYC_ASSERT(error.IsErrorToken());
    MYC_ASSERT(error.GetCategory() == myc::lexer::TokenCategory::Error);
    MYC_ASSERT(error.HasFlag(myc::lexer::TokenFlags::Invalid));
    MYC_ASSERT_EQ(myc::lexer::TokenPrinter::Print(error), "Error(\"@\")");
}

MYC_TEST(token_type_string_mapping) {
    MYC_ASSERT_EQ(std::string(myc::lexer::ToString(myc::lexer::TokenType::KwWhile)), "KwWhile");
    MYC_ASSERT_EQ(std::string(myc::lexer::ToString(myc::lexer::TokenType::DelimLeftBrace)),
                  "DelimLeftBrace");
}

MYC_TEST(literal_kind_helpers) {
    MYC_ASSERT(myc::lexer::IsNumericLiteral(myc::lexer::LiteralKind::Hexadecimal));
    MYC_ASSERT(!myc::lexer::IsNumericLiteral(myc::lexer::LiteralKind::String));
}
