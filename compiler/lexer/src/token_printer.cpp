#include "myc/lexer/token_printer.hpp"

#include "myc/lexer/token_type.hpp"

#include <sstream>

namespace myc::lexer {

namespace {

[[nodiscard]] std::string EscapeLexeme(std::string_view lexeme) {
    std::string escaped;
    escaped.reserve(lexeme.size() + 2);
    for (const char ch : lexeme) {
        switch (ch) {
            case '\\':
                escaped += "\\\\";
                break;
            case '"':
                escaped += "\\\"";
                break;
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            case '\t':
                escaped += "\\t";
                break;
            default:
                escaped += ch;
                break;
        }
    }
    return escaped;
}

[[nodiscard]] std::string DisplayName(const Token& token) {
    if (token.IsEOF()) {
        return "EOF";
    }
    if (token.IsErrorToken()) {
        return "Error";
    }
    if (token.IsKeyword()) {
        return "Keyword";
    }
    if (token.IsPrimitiveType()) {
        return "PrimitiveType";
    }
    if (token.IsLiteral()) {
        switch (token.GetLiteralKind()) {
            case LiteralKind::Integer:
            case LiteralKind::Hexadecimal:
            case LiteralKind::Binary:
            case LiteralKind::Octal:
                return "IntegerLiteral";
            case LiteralKind::Float:
            case LiteralKind::Scientific:
                return "FloatLiteral";
            case LiteralKind::Boolean:
                return "BooleanLiteral";
            case LiteralKind::Character:
                return "CharacterLiteral";
            case LiteralKind::String:
            case LiteralKind::RawString:
            case LiteralKind::MultilineString:
            case LiteralKind::InterpolatedString:
                return "StringLiteral";
            default:
                return std::string(ToString(token.GetType()));
        }
    }
    if (token.IsOperator()) {
        return "Operator";
    }
    if (token.IsDelimiter()) {
        return std::string(ToString(token.GetType()));
    }
    if (token.IsPunctuation()) {
        return std::string(ToString(token.GetType()));
    }
    if (token.GetType() == TokenType::Identifier) {
        return "Identifier";
    }
    return std::string(ToString(token.GetType()));
}

}  // namespace

std::string TokenPrinter::Print(const Token& token) {
    const std::string name = DisplayName(token);
    if (token.IsEOF()) {
        return name;
    }
    if (token.GetLexeme().empty()) {
        return name + "()";
    }
    return name + "(\"" + EscapeLexeme(token.GetLexeme()) + "\")";
}

std::string TokenPrinter::PrintType(const Token& token) {
    return std::string(ToString(token.GetType()));
}

std::string TokenPrinter::PrintWithLocation(const Token& token) {
    std::ostringstream stream;
    stream << Print(token) << " @ " << token.GetLocation().GetLine() << ':'
           << token.GetLocation().GetColumn();
    return stream.str();
}

void TokenPrinter::PrintTo(std::ostream& output, const Token& token) {
    output << Print(token);
}

}  // namespace myc::lexer
