#pragma once

#include "myc/ast/declaration_nodes.hpp"
#include "myc/parser/parser_context.hpp"

#include <memory>
#include <vector>

namespace myc::parser {

/// Parses attribute annotations such as @inline.
class AttributeParser {
public:
    explicit AttributeParser(ParserContext& context);

    [[nodiscard]] std::vector<std::unique_ptr<ast::AttributeNode>> ParseAttributes();

private:
    ParserContext& context_;
};

}  // namespace myc::parser
