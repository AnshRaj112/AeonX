#pragma once

#include "myc/ast/declaration_nodes.hpp"
#include "myc/parser/parser_context.hpp"

#include <memory>
#include <vector>

namespace myc::parser {

/// Parses import declarations.
class ImportParser {
public:
    explicit ImportParser(ParserContext& context);

    [[nodiscard]] std::unique_ptr<ast::ImportDeclarationNode> ParseImport();

private:
    [[nodiscard]] std::vector<std::unique_ptr<ast::IdentifierNode>> ParsePath();

    ParserContext& context_;
};

}  // namespace myc::parser
