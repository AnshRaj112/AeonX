#include "myc/source/source_buffer.hpp"

#include <stdexcept>

namespace myc::source {

SourceBuffer::SourceBuffer(SourceFileID id, std::string path, std::string content,
                           LineTable line_table, bool valid_utf8)
    : id_(id),
      path_(std::move(path)),
      content_(std::move(content)),
      line_table_(std::move(line_table)),
      valid_utf8_(valid_utf8) {}

SourceFileID SourceBuffer::GetId() const noexcept { return id_; }

FileId SourceBuffer::GetFileId() const noexcept { return id_.Value(); }

const std::string& SourceBuffer::GetPath() const noexcept { return path_; }

std::string_view SourceBuffer::GetContent() const noexcept { return content_; }

std::size_t SourceBuffer::GetSize() const noexcept { return content_.size(); }

bool SourceBuffer::IsValidUtf8() const noexcept { return valid_utf8_; }

const LineTable& SourceBuffer::GetLineTable() const noexcept { return line_table_; }

char SourceBuffer::At(std::size_t offset) const {
  if (offset >= content_.size()) {
    throw std::out_of_range("SourceBuffer::At offset out of range");
  }
  return content_[offset];
}

std::optional<char> SourceBuffer::TryAt(std::size_t offset) const noexcept {
  if (offset >= content_.size()) {
    return std::nullopt;
  }
  return content_[offset];
}

std::string_view SourceBuffer::Substring(std::size_t start, std::size_t length) const {
  ValidateRange(start, start + length);
  return std::string_view(content_).substr(start, length);
}

std::string_view SourceBuffer::Slice(std::size_t start, std::size_t end) const {
  ValidateRange(start, end);
  return std::string_view(content_).substr(start, end - start);
}

std::pair<LineNumber, ColumnNumber> SourceBuffer::GetLineAndColumn(
    std::size_t offset) const noexcept {
  if (offset > content_.size()) {
    return {line_table_.GetLineCount(), 1};
  }
  return line_table_.GetLineAndColumn(offset);
}

std::size_t SourceBuffer::GetLineStartOffset(LineNumber line) const noexcept {
  return line_table_.GetLineStartOffset(line);
}

LineNumber SourceBuffer::GetLineCount() const noexcept { return line_table_.GetLineCount(); }

std::string_view SourceBuffer::GetLine(LineNumber line) const noexcept {
  return line_table_.GetLine(content_, line);
}

void SourceBuffer::ValidateRange(std::size_t start, std::size_t end) const {
  if (start > end || end > content_.size()) {
    throw std::out_of_range("SourceBuffer range out of bounds");
  }
}

}  // namespace myc::source
