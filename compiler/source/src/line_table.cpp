#include "myc/source/line_table.hpp"

#include <algorithm>

namespace myc::source {

LineTable::LineTable(std::string_view content) {
  line_starts_.push_back(0);
  for (std::size_t i = 0; i < content.size(); ++i) {
    if (content[i] == '\n') {
      line_starts_.push_back(i + 1);
    }
  }
}

LineNumber LineTable::GetLineCount() const noexcept {
  return static_cast<LineNumber>(line_starts_.size());
}

std::pair<LineNumber, ColumnNumber> LineTable::GetLineAndColumn(
    std::size_t offset) const noexcept {
  if (line_starts_.empty()) {
    return {1, 1};
  }
  const auto it = std::upper_bound(line_starts_.begin(), line_starts_.end(), offset);
  const std::size_t line_index =
      static_cast<std::size_t>(std::distance(line_starts_.begin(), it)) - 1;
  const LineNumber line = static_cast<LineNumber>(line_index + 1);
  const std::size_t line_start = line_starts_[line_index];
  const ColumnNumber column = static_cast<ColumnNumber>(offset - line_start + 1);
  return {line, column};
}

std::size_t LineTable::GetLineStartOffset(LineNumber line) const noexcept {
  if (line == 0 || line > line_starts_.size()) {
    return 0;
  }
  return line_starts_[static_cast<std::size_t>(line - 1)];
}

std::size_t LineTable::GetLineEndOffset(LineNumber line,
                                        std::size_t content_size) const noexcept {
  if (line == 0 || line > line_starts_.size()) {
    return 0;
  }
  if (line < line_starts_.size()) {
    return line_starts_[static_cast<std::size_t>(line)];
  }
  return content_size;
}

std::string_view LineTable::GetLine(std::string_view content, LineNumber line) const noexcept {
  const std::size_t start = GetLineStartOffset(line);
  if (start >= content.size()) {
    return {};
  }
  const std::size_t end = GetLineEndOffset(line, content.size());
  std::string_view line_view = content.substr(start, end - start);
  while (!line_view.empty() && (line_view.back() == '\n' || line_view.back() == '\r')) {
    line_view.remove_suffix(1);
  }
  return line_view;
}

}  // namespace myc::source
