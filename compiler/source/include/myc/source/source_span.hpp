#pragma once

#include "myc/source/source_location.hpp"

#include <compare>
#include <cstddef>
#include <optional>
#include <string>

namespace myc::source {

/// A half-open byte range inside a source file.
class SourceSpan {
 public:
  SourceSpan() = default;

  SourceSpan(SourceLocation start, SourceLocation end) : start_(start), end_(end) {}

  [[nodiscard]] const SourceLocation& GetStart() const noexcept { return start_; }
  [[nodiscard]] const SourceLocation& GetEnd() const noexcept { return end_; }

  [[nodiscard]] std::size_t GetLength() const noexcept {
    if (!IsValid() || end_.GetOffset() < start_.GetOffset()) {
      return 0;
    }
    return end_.GetOffset() - start_.GetOffset();
  }

  [[nodiscard]] bool IsValid() const noexcept {
    return start_.IsValid() && end_.IsValid() &&
           start_.GetFileId() == end_.GetFileId();
  }

  [[nodiscard]] bool IsEmpty() const noexcept { return GetLength() == 0; }

  [[nodiscard]] bool Contains(const SourceLocation& location) const noexcept;
  [[nodiscard]] bool Contains(std::size_t offset) const noexcept;
  [[nodiscard]] bool Intersects(const SourceSpan& other) const noexcept;

  /// Returns the smallest span covering both ranges in the same file.
  [[nodiscard]] std::optional<SourceSpan> Merge(const SourceSpan& other) const noexcept;

  /// Human-readable representation for debugging and future syntax highlighting.
  [[nodiscard]] std::string ToString() const;

  [[nodiscard]] auto operator<=>(const SourceSpan& other) const noexcept = default;
  [[nodiscard]] bool operator==(const SourceSpan& other) const noexcept = default;

 private:
  SourceLocation start_;
  SourceLocation end_;
};

}  // namespace myc::source
