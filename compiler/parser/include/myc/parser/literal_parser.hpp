#pragma once

#include "myc/ast/literal_nodes.hpp"
#include "myc/parser/parser_context.hpp"

#include <memory>

namespace myc::parser {

/// Parses literal tokens into AST literal nodes.
class LiteralParser {
public:
    explicit LiteralParser(ParserContext& context);

    [[nodiscard]] std::unique_ptr<ast::LiteralNode> ParseLiteral();

private:
    ParserContext& context_;
};

}  // namespace myc::parser
