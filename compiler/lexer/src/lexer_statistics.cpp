#include "myc/lexer/lexer_statistics.hpp"

namespace myc::lexer {

void LexerStatistics::Reset() noexcept {
    characters_read_ = 0;
    lines_processed_ = 0;
    scanner_calls_ = 0;
    tokens_produced_ = 0;
    errors_encountered_ = 0;
    memory_usage_bytes_ = 0;
    start_time_ = TimePoint{};
    end_time_ = TimePoint{};
}

}  // namespace myc::lexer
