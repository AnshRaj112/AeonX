#include "myc/parser/parser.hpp"

#include "myc/ast/program_node.hpp"
#include "myc/parser/declaration_parser.hpp"

namespace myc::parser {

Parser::Parser(lexer::TokenStream tokens, source::SourceManager& source_manager,
               diagnostics::DiagnosticEmitter& diagnostics, ParserConfiguration configuration)
    : tokens_(std::move(tokens)),
      source_manager_(source_manager),
      diagnostics_(diagnostics),
      configuration_(configuration) {}

ParserResult Parser::ParseProgram() {
    ParserResult result;
    if (configuration_.collect_statistics) {
        result.statistics.BeginTiming();
    }

    ParserContext context(tokens_, source_manager_, diagnostics_, configuration_);
    DeclarationParser declaration_parser(context);

    auto declarations = declaration_parser.ParseDeclarations();
    source::SourceSpan span;
    if (!declarations.empty()) {
        span = declarations.front()->GetSpan();
        for (const auto& declaration : declarations) {
            span = ast::ExtendSpan(span, declaration->GetSpan());
        }
    }

    result.program = std::make_unique<ast::ProgramNode>(std::move(declarations), span);
    result.had_errors = context.GetState().had_error;
    result.statistics = context.GetStatistics();

    if (configuration_.collect_statistics) {
        result.statistics.EndTiming();
    }

    return result;
}

}  // namespace myc::parser
