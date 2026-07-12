#pragma once

#include "myc/source/source_file_id.hpp"
#include "myc/types.hpp"

#include <compare>
#include <cstddef>

namespace myc::source {

/// An exact position inside a source file.
class SourceLocation {
 public:
  SourceLocation() = default;

  SourceLocation(SourceFileID file_id, std::size_t offset, LineNumber line,
                 ColumnNumber column) noexcept
      : file_id_(file_id), offset_(offset), line_(line), column_(column) {}

  [[nodiscard]] SourceFileID GetFileId() const noexcept { return file_id_; }
  [[nodiscard]] std::size_t GetOffset() const noexcept { return offset_; }
  [[nodiscard]] LineNumber GetLine() const noexcept { return line_; }
  [[nodiscard]] ColumnNumber GetColumn() const noexcept { return column_; }

  [[nodiscard]] bool IsValid() const noexcept { return file_id_.IsValid(); }

  [[nodiscard]] auto operator<=>(const SourceLocation& other) const noexcept = default;
  [[nodiscard]] bool operator==(const SourceLocation& other) const noexcept = default;

  /// Returns a location at a different byte offset within the same file.
  [[nodiscard]] SourceLocation WithOffset(std::size_t offset) const noexcept {
    return SourceLocation(file_id_, offset, line_, column_);
  }

  /// Returns a location with updated line/column metadata.
  [[nodiscard]] SourceLocation WithLineColumn(LineNumber line,
                                              ColumnNumber column) const noexcept {
    return SourceLocation(file_id_, offset_, line, column);
  }

 private:
  SourceFileID file_id_;
  std::size_t offset_{0};
  LineNumber line_{1};
  ColumnNumber column_{1};
};

}  // namespace myc::source
