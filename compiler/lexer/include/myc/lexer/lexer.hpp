#pragma once

#include "myc/lexer/character_reader.hpp"
#include "myc/lexer/character_stream.hpp"
#include "myc/lexer/lexer_configuration.hpp"
#include "myc/lexer/lexer_context.hpp"
#include "myc/lexer/lexer_options.hpp"
#include "myc/lexer/lexer_result.hpp"
#include "myc/lexer/lexer_statistics.hpp"
#include "myc/lexer/scanners/scanner_registry.hpp"
#include "myc/lexer/token_factory.hpp"
#include "myc/lexer/token_stream.hpp"

#include <vector>

namespace myc::diagnostics {
class DiagnosticEmitter;
}

namespace myc::source {
class SourceBuffer;
class SourceManager;
}

namespace myc::lexer {

/// Coordinator for lexical analysis. Owns character streaming and scanner dispatch.
///
/// The lexer does not embed tokenization logic directly. Specialized scanners
/// perform classification; this class orchestrates the scan loop, diagnostics,
/// and TokenStream construction.
class Lexer {
public:
    Lexer(const source::SourceBuffer& buffer, source::SourceManager& source_manager,
          diagnostics::DiagnosticEmitter& diagnostics, LexerOptions options = {});

    /// Runs the scan loop and returns tokens with collected statistics.
    ///
    /// Tokenization rules are not implemented yet. The loop exercises character
    /// streaming and scanner dispatch, then appends a terminating EOF token.
    [[nodiscard]] LexerResult Tokenize();

    [[nodiscard]] const LexerStatistics& GetStatistics() const noexcept { return statistics_; }
    [[nodiscard]] CharacterStream& GetCharacterStream() noexcept { return stream_; }
    [[nodiscard]] const CharacterStream& GetCharacterStream() const noexcept { return stream_; }

private:
    void RecoverFromUnknownCharacter(LexerContext& context);
    [[nodiscard]] Token MakeEofToken() const;

    CharacterReader reader_;
    CharacterStream stream_;
    source::SourceManager& source_manager_;
    diagnostics::DiagnosticEmitter& diagnostics_;
    LexerConfiguration configuration_;
    LexerStatistics statistics_;
    ScannerRegistry scanners_;
};

}  // namespace myc::lexer
