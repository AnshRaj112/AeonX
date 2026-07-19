#pragma once

#include "myc/ast/type_nodes.hpp"
#include "myc/parser/parser_context.hpp"

#include <memory>

namespace myc::parser {

/// Parses type annotations.
class TypeParser {
public:
    explicit TypeParser(ParserContext& context);

    [[nodiscard]] std::unique_ptr<ast::TypeNode> ParseType();

private:
    [[nodiscard]] bool IsTypeStart(lexer::TokenType type) const noexcept;

    ParserContext& context_;
};

}  // namespace myc::parser
