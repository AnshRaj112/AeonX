#pragma once

namespace myc::parser {

/// Tracks parser phase state for incremental parsing extensions.
struct ParserState {
    bool in_panic_mode{false};
    bool had_error{false};
    int error_count{0};
    bool recovered{false};
};

}  // namespace myc::parser
