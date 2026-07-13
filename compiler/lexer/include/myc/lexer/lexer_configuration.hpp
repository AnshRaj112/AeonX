#pragma once

#include "myc/lexer/future_incremental_lexing.hpp"
#include "myc/lexer/lexer_options.hpp"

namespace myc::lexer {

/// Resolved lexer settings used during scanning.
class LexerConfiguration {
public:
    LexerConfiguration() = default;
    explicit LexerConfiguration(LexerOptions options);

    [[nodiscard]] const LexerOptions& GetOptions() const noexcept { return options_; }
    [[nodiscard]] bool ShouldCollectStatistics() const noexcept {
        return options_.collect_statistics;
    }
    [[nodiscard]] bool ShouldContinueAfterErrors() const noexcept {
        return options_.continue_after_errors;
    }
    [[nodiscard]] bool ShouldSkipWhitespace() const noexcept { return options_.skip_whitespace; }
    [[nodiscard]] bool ShouldIncludeTrivia() const noexcept { return options_.include_trivia; }
    [[nodiscard]] bool AreDirectivesEnabled() const noexcept { return options_.enable_directives; }
    [[nodiscard]] bool AreAnnotationsEnabled() const noexcept {
        return options_.enable_annotations;
    }

    [[nodiscard]] const FutureIncrementalLexing& GetIncrementalLexing() const noexcept {
        return incremental_;
    }
    [[nodiscard]] FutureIncrementalLexing& GetIncrementalLexing() noexcept { return incremental_; }

private:
    LexerOptions options_{};
    FutureIncrementalLexing incremental_{};
};

}  // namespace myc::lexer
