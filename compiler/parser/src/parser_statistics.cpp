#include "myc/parser/parser_statistics.hpp"

namespace myc::parser {

void ParserStatistics::Reset() noexcept {
    nodes_created_ = 0;
    declarations_parsed_ = 0;
    statements_parsed_ = 0;
    expressions_parsed_ = 0;
    syntax_errors_ = 0;
    start_time_ = {};
    end_time_ = {};
}

}  // namespace myc::parser
