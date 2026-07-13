#pragma once

namespace myc::lexer {

/// User-facing lexer toggles supplied at construction time.
struct LexerOptions {
    bool collect_statistics{true};
    bool continue_after_errors{true};
    bool skip_whitespace{false};
    bool include_trivia{false};
    bool enable_directives{true};
    bool enable_annotations{true};
};

}  // namespace myc::lexer
