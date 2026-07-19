#pragma once

#include <chrono>
#include <cstddef>

namespace myc::parser {

/// Runtime counters collected during parsing.
class ParserStatistics {
public:
    void RecordNodeCreated() noexcept { ++nodes_created_; }
    void RecordDeclarationParsed() noexcept { ++declarations_parsed_; }
    void RecordStatementParsed() noexcept { ++statements_parsed_; }
    void RecordExpressionParsed() noexcept { ++expressions_parsed_; }
    void RecordSyntaxError() noexcept { ++syntax_errors_; }

    void BeginTiming() noexcept { start_time_ = Clock::now(); }
    void EndTiming() noexcept { end_time_ = Clock::now(); }

    [[nodiscard]] std::size_t GetNodesCreated() const noexcept { return nodes_created_; }
    [[nodiscard]] std::size_t GetDeclarationsParsed() const noexcept {
        return declarations_parsed_;
    }
    [[nodiscard]] std::size_t GetStatementsParsed() const noexcept { return statements_parsed_; }
    [[nodiscard]] std::size_t GetExpressionsParsed() const noexcept { return expressions_parsed_; }
    [[nodiscard]] std::size_t GetSyntaxErrors() const noexcept { return syntax_errors_; }

    [[nodiscard]] std::chrono::nanoseconds GetElapsedTime() const noexcept {
        if (end_time_ < start_time_) {
            return std::chrono::nanoseconds{0};
        }
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end_time_ - start_time_);
    }

    void Reset() noexcept;

private:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    std::size_t nodes_created_{0};
    std::size_t declarations_parsed_{0};
    std::size_t statements_parsed_{0};
    std::size_t expressions_parsed_{0};
    std::size_t syntax_errors_{0};
    TimePoint start_time_{};
    TimePoint end_time_{};
};

}  // namespace myc::parser
