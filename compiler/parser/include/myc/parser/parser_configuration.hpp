#pragma once

namespace myc::parser {

/// Parser behavior configuration.
struct ParserConfiguration {
    bool enable_error_recovery{true};
    bool allow_missing_semicolons{false};
    int max_syntax_errors{256};
    bool collect_statistics{true};
};

}  // namespace myc::parser
