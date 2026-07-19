#pragma once

#include "myc/lexer/token_stream.hpp"
#include "myc/parser/parser_configuration.hpp"
#include "myc/parser/parser_result.hpp"

#include "myc/diagnostics/diagnostic_emitter.hpp"
#include "myc/source/source_manager.hpp"

namespace myc::parser {

/// Top-level parser entry point. Delegates to specialized parser components.
class Parser {
public:
    Parser(lexer::TokenStream tokens, source::SourceManager& source_manager,
           diagnostics::DiagnosticEmitter& diagnostics,
           ParserConfiguration configuration = {});

    [[nodiscard]] ParserResult ParseProgram();

private:
    lexer::TokenStream tokens_;
    source::SourceManager& source_manager_;
    diagnostics::DiagnosticEmitter& diagnostics_;
    ParserConfiguration configuration_;
};

}  // namespace myc::parser
