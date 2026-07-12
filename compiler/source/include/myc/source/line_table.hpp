#pragma once

#include "myc/types.hpp"

#include <cstddef>
#include <string_view>
#include <utility>
#include <vector>

namespace myc::source {

/// Precomputed line-start offsets for fast offset ↔ line/column mapping.
class LineTable {
 public:
  LineTable() = default;

  /// Builds the line table from source text. Supports LF and CRLF endings.
  explicit LineTable(std::string_view content);

  [[nodiscard]] LineNumber GetLineCount() const noexcept;

  /// Maps a byte offset to a 1-based line and column.
  [[nodiscard]] std::pair<LineNumber, ColumnNumber> GetLineAndColumn(
      std::size_t offset) const noexcept;

  /// Returns the byte offset of the start of a 1-based line.
  [[nodiscard]] std::size_t GetLineStartOffset(LineNumber line) const noexcept;

  /// Returns the byte offset one past the end of a 1-based line.
  [[nodiscard]] std::size_t GetLineEndOffset(LineNumber line,
                                               std::size_t content_size) const noexcept;

  /// Returns a view of a single line without the trailing newline characters.
  [[nodiscard]] std::string_view GetLine(std::string_view content,
                                         LineNumber line) const noexcept;

  [[nodiscard]] const std::vector<std::size_t>& GetLineStarts() const noexcept {
    return line_starts_;
  }

 private:
  std::vector<std::size_t> line_starts_;
};

}  // namespace myc::source
