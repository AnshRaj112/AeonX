#pragma once

#include "myc/source/line_table.hpp"
#include "myc/source/source_file_id.hpp"
#include "myc/types.hpp"

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace myc::source {

/// Efficient read-only view over immutable source text.
class SourceBuffer {
 public:
  SourceBuffer(SourceFileID id, std::string path, std::string content, LineTable line_table,
               bool valid_utf8);

  [[nodiscard]] SourceFileID GetId() const noexcept;
  [[nodiscard]] FileId GetFileId() const noexcept;
  [[nodiscard]] const std::string& GetPath() const noexcept;
  [[nodiscard]] std::string_view GetContent() const noexcept;
  [[nodiscard]] std::size_t GetSize() const noexcept;

  [[nodiscard]] bool IsValidUtf8() const noexcept;
  [[nodiscard]] const LineTable& GetLineTable() const noexcept;

  /// Returns the byte at offset. Throws std::out_of_range when out of bounds.
  [[nodiscard]] char At(std::size_t offset) const;

  /// Returns the byte at offset, or std::nullopt when out of bounds.
  [[nodiscard]] std::optional<char> TryAt(std::size_t offset) const noexcept;

  /// Returns a substring view. Throws when the range is invalid.
  [[nodiscard]] std::string_view Substring(std::size_t start, std::size_t length) const;

  /// Returns a half-open slice [start, end). Throws when the range is invalid.
  [[nodiscard]] std::string_view Slice(std::size_t start, std::size_t end) const;

  [[nodiscard]] std::pair<LineNumber, ColumnNumber> GetLineAndColumn(
      std::size_t offset) const noexcept;

  [[nodiscard]] std::size_t GetLineStartOffset(LineNumber line) const noexcept;
  [[nodiscard]] LineNumber GetLineCount() const noexcept;
  [[nodiscard]] std::string_view GetLine(LineNumber line) const noexcept;

 private:
  void ValidateRange(std::size_t start, std::size_t end) const;

  SourceFileID id_;
  std::string path_;
  std::string content_;
  LineTable line_table_;
  bool valid_utf8_{true};
};

}  // namespace myc::source
