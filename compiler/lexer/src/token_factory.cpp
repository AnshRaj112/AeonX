#include "myc/lexer/token_category_map.hpp"
#include "myc/lexer/token_factory.hpp"

namespace myc::lexer {

namespace {

[[nodiscard]] std::size_t ComputeLength(std::string_view lexeme,
                                        const source::SourceSpan& span) noexcept {
    if (const std::size_t span_length = span.GetLength(); span_length > 0) {
        return span_length;
    }
    return lexeme.size();
}

}  // namespace

Token TokenFactory::Build(TokenType type, std::string_view lexeme,
                          source::SourceLocation location, source::SourceSpan span,
                          TokenFlags flags, LiteralKind literal_kind) {
    const std::size_t length = ComputeLength(lexeme, span);
    return Token(type, lexeme, location, span, length, flags, GetCategory(type), literal_kind,
                 FutureSemanticValue{}, FutureMacroToken{}, FutureTrivia{});
}

Token TokenFactory::Make(TokenType type, std::string_view lexeme,
                         source::SourceLocation location, source::SourceSpan span,
                         TokenFlags flags, LiteralKind literal_kind) {
    return Build(type, lexeme, location, span, flags, literal_kind);
}

Token TokenFactory::MakeIdentifier(std::string_view lexeme, source::SourceLocation location,
                                   source::SourceSpan span, TokenFlags flags) {
    return Build(TokenType::Identifier, lexeme, location, span, flags, LiteralKind::None);
}

Token TokenFactory::MakeKeyword(TokenType type, std::string_view lexeme,
                                source::SourceLocation location, source::SourceSpan span,
                                TokenFlags flags) {
    return Build(type, lexeme, location, span, flags, LiteralKind::None);
}

Token TokenFactory::MakeLiteral(TokenType type, LiteralKind literal_kind,
                                std::string_view lexeme, source::SourceLocation location,
                                source::SourceSpan span, TokenFlags flags) {
    return Build(type, lexeme, location, span, flags, literal_kind);
}

Token TokenFactory::MakeOperator(TokenType type, std::string_view lexeme,
                                 source::SourceLocation location, source::SourceSpan span,
                                 TokenFlags flags) {
    return Build(type, lexeme, location, span, flags, LiteralKind::None);
}

Token TokenFactory::MakePunctuation(TokenType type, std::string_view lexeme,
                                    source::SourceLocation location, source::SourceSpan span,
                                    TokenFlags flags) {
    return Build(type, lexeme, location, span, flags, LiteralKind::None);
}

Token TokenFactory::MakeDelimiter(TokenType type, std::string_view lexeme,
                                  source::SourceLocation location, source::SourceSpan span,
                                  TokenFlags flags) {
    return Build(type, lexeme, location, span, flags, LiteralKind::None);
}

Token TokenFactory::MakeEOF(source::SourceLocation location, source::SourceSpan span) {
    return Build(TokenType::EndOfFile, std::string_view{}, location, span, TokenFlags::None,
                 LiteralKind::None);
}

Token TokenFactory::MakeError(std::string_view lexeme, source::SourceLocation location,
                              source::SourceSpan span, TokenFlags flags) {
    return Build(TokenType::Error, lexeme, location, span, flags | TokenFlags::Invalid,
                 LiteralKind::None);
}

Token TokenFactory::MakeUnknown(std::string_view lexeme, source::SourceLocation location,
                                source::SourceSpan span, TokenFlags flags) {
    return Build(TokenType::Unknown, lexeme, location, span, flags, LiteralKind::None);
}

std::vector<Token> TokenFactory::MakeBatch(std::initializer_list<Token> tokens) {
    return std::vector<Token>(tokens);
}

}  // namespace myc::lexer
