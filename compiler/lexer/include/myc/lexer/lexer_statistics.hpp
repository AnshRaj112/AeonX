#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>

namespace myc::lexer {

/// Runtime counters collected during lexical analysis.
class LexerStatistics {
public:
    void RecordCharacterRead() noexcept { ++characters_read_; }
    void RecordLineProcessed() noexcept { ++lines_processed_; }
    void RecordScannerCall() noexcept { ++scanner_calls_; }
    void RecordTokenProduced() noexcept { ++tokens_produced_; }
    void RecordError() noexcept { ++errors_encountered_; }

    void BeginTiming() noexcept { start_time_ = Clock::now(); }
    void EndTiming() noexcept { end_time_ = Clock::now(); }

    [[nodiscard]] std::size_t GetCharactersRead() const noexcept { return characters_read_; }
    [[nodiscard]] std::size_t GetLinesProcessed() const noexcept { return lines_processed_; }
    [[nodiscard]] std::size_t GetScannerCalls() const noexcept { return scanner_calls_; }
    [[nodiscard]] std::size_t GetTokensProduced() const noexcept { return tokens_produced_; }
    [[nodiscard]] std::size_t GetErrorsEncountered() const noexcept { return errors_encountered_; }

    /// Placeholder for future memory profiling integration.
    [[nodiscard]] std::uint64_t GetMemoryUsageBytes() const noexcept { return memory_usage_bytes_; }
    void SetMemoryUsageBytes(std::uint64_t bytes) noexcept { memory_usage_bytes_ = bytes; }

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

    std::size_t characters_read_{0};
    std::size_t lines_processed_{0};
    std::size_t scanner_calls_{0};
    std::size_t tokens_produced_{0};
    std::size_t errors_encountered_{0};
    std::uint64_t memory_usage_bytes_{0};
    TimePoint start_time_{};
    TimePoint end_time_{};
};

}  // namespace myc::lexer
