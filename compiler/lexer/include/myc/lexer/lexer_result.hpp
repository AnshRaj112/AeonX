#pragma once

#include "myc/lexer/lexer_statistics.hpp"
#include "myc/lexer/token_stream.hpp"

namespace myc::lexer {

/// Output of a full lexing pass.
struct LexerResult {
    TokenStream tokens;
    LexerStatistics statistics;
    bool had_errors{false};

    [[nodiscard]] bool IsEmpty() const noexcept { return tokens.Empty(); }
};

}  // namespace myc::lexer
