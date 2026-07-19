#pragma once

#include "myc/ast/declaration_nodes.hpp"
#include "myc/parser/attribute_parser.hpp"
#include "myc/parser/import_parser.hpp"
#include "myc/parser/parser_context.hpp"
#include "myc/parser/statement_parser.hpp"
#include "myc/parser/type_parser.hpp"
#include "myc/parser/recovery_manager.hpp"

#include <memory>
#include <vector>

namespace myc::parser {

/// Parses top-level and nested declarations.
class DeclarationParser {
public:
    explicit DeclarationParser(ParserContext& context);

    [[nodiscard]] std::unique_ptr<ast::DeclarationNode> ParseDeclaration();
    [[nodiscard]] std::vector<std::unique_ptr<ast::DeclarationNode>> ParseDeclarations();

private:
    [[nodiscard]] bool IsDeclarationStart() const;
    [[nodiscard]] std::unique_ptr<ast::DeclarationNode> ParseFunctionDeclaration(
        bool is_public, bool is_async,
        std::vector<std::unique_ptr<ast::AttributeNode>> attributes);
    [[nodiscard]] std::unique_ptr<ast::DeclarationNode> ParseStructDeclaration(bool is_public);
    [[nodiscard]] std::unique_ptr<ast::DeclarationNode> ParseClassDeclaration(bool is_public);
    [[nodiscard]] std::unique_ptr<ast::DeclarationNode> ParseTraitDeclaration(bool is_public);
    [[nodiscard]] std::unique_ptr<ast::DeclarationNode> ParseEnumDeclaration(bool is_public);
    [[nodiscard]] std::unique_ptr<ast::DeclarationNode> ParseModuleDeclaration(bool is_public);
    [[nodiscard]] std::unique_ptr<ast::DeclarationNode> ParsePackageDeclaration();
    [[nodiscard]] std::unique_ptr<ast::IdentifierNode> ParseIdentifier();
    [[nodiscard]] std::vector<std::unique_ptr<ast::ParameterNode>> ParseParameters();
    [[nodiscard]] std::vector<std::unique_ptr<ast::FieldNode>> ParseFields();

    ParserContext& context_;
    StatementParser statement_parser_;
    TypeParser type_parser_;
    AttributeParser attribute_parser_;
    ImportParser import_parser_;
};

}  // namespace myc::parser
