#include "myc/parser/future_generic_parser.hpp"

#include "myc/lexer/token_type.hpp"

namespace myc::parser {

FutureGenericParser::FutureGenericParser(ParserContext& context) : context_(context) {}

bool FutureGenericParser::IsGenericStart() const {
    return context_.Peek().Is(lexer::TokenType::DelimLeftAngle);
}

}  // namespace myc::parser
