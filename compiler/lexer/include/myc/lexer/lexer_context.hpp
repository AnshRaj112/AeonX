#pragma once

#include "myc/lexer/character_stream.hpp"
#include "myc/lexer/lexer_configuration.hpp"
#include "myc/lexer/lexer_state.hpp"
#include "myc/lexer/lexer_statistics.hpp"
#include "myc/lexer/token.hpp"

#include <vector>

namespace myc::diagnostics {
class DiagnosticEmitter;
}

namespace myc::source {
class SourceManager;
}

namespace myc::lexer {

/// Per-scan mutable state shared between the lexer coordinator and scanners.
class LexerContext {
public:
    LexerContext(CharacterStream& stream, source::SourceManager& source_manager,
                 diagnostics::DiagnosticEmitter& diagnostics, LexerConfiguration configuration,
                 LexerStatistics& statistics);

    [[nodiscard]] CharacterStream& GetStream() noexcept { return stream_; }
    [[nodiscard]] const CharacterStream& GetStream() const noexcept { return stream_; }

    [[nodiscard]] source::SourceManager& GetSourceManager() noexcept { return source_manager_; }
    [[nodiscard]] const source::SourceManager& GetSourceManager() const noexcept {
        return source_manager_;
    }

    [[nodiscard]] diagnostics::DiagnosticEmitter& GetDiagnostics() noexcept { return diagnostics_; }
    [[nodiscard]] const diagnostics::DiagnosticEmitter& GetDiagnostics() const noexcept {
        return diagnostics_;
    }

    [[nodiscard]] LexerConfiguration& GetConfiguration() noexcept { return configuration_; }
    [[nodiscard]] const LexerConfiguration& GetConfiguration() const noexcept {
        return configuration_;
    }

    [[nodiscard]] LexerStatistics& GetStatistics() noexcept { return statistics_; }
    [[nodiscard]] const LexerStatistics& GetStatistics() const noexcept { return statistics_; }

    [[nodiscard]] LexerState GetState() const noexcept { return state_; }
    void SetState(LexerState state) noexcept { state_ = state; }

    [[nodiscard]] std::vector<Token>& GetPendingTokens() noexcept { return pending_tokens_; }
    [[nodiscard]] const std::vector<Token>& GetPendingTokens() const noexcept {
        return pending_tokens_;
    }

    void PushToken(Token token);
    void ClearPendingTokens() noexcept { pending_tokens_.clear(); }

    [[nodiscard]] bool HadErrors() const noexcept { return had_errors_; }
    void MarkError() noexcept { had_errors_ = true; }

    [[nodiscard]] source::SourceFileID GetFileId() const noexcept;

private:
    CharacterStream& stream_;
    source::SourceManager& source_manager_;
    diagnostics::DiagnosticEmitter& diagnostics_;
    LexerConfiguration configuration_;
    LexerStatistics& statistics_;
    LexerState state_{LexerState::Normal};
    std::vector<Token> pending_tokens_;
    bool had_errors_{false};
};

}  // namespace myc::lexer
