#include "myc/parser/future_macro_parser.hpp"

#include "myc/lexer/token_type.hpp"

namespace myc::parser {

FutureMacroParser::FutureMacroParser(ParserContext& context) : context_(context) {}

bool FutureMacroParser::IsMacroInvocation() const {
    return context_.Peek().Is(lexer::TokenType::Identifier) &&
           context_.LookAhead(1).Is(lexer::TokenType::DelimLeftParen);
}

}  // namespace myc::parser
