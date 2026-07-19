#pragma once

#include "myc/ast/program_node.hpp"
#include "myc/parser/parser_statistics.hpp"

#include <memory>

namespace myc::parser {

/// Output of a full parsing pass.
struct ParserResult {
    std::unique_ptr<ast::ProgramNode> program;
    ParserStatistics statistics;
    bool had_errors{false};

    [[nodiscard]] bool IsValid() const noexcept {
        return program != nullptr && !had_errors;
    }
};

}  // namespace myc::parser
