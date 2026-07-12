#include "myc/source/source_span.hpp"

#include <format>
#include <utility>

namespace myc::source {

bool SourceSpan::Contains(const SourceLocation& location) const noexcept {
  if (!IsValid() || location.GetFileId() != start_.GetFileId()) {
    return false;
  }
  const std::size_t offset = location.GetOffset();
  return offset >= start_.GetOffset() && offset < end_.GetOffset();
}

bool SourceSpan::Contains(std::size_t offset) const noexcept {
  if (!IsValid()) {
    return false;
  }
  return offset >= start_.GetOffset() && offset < end_.GetOffset();
}

bool SourceSpan::Intersects(const SourceSpan& other) const noexcept {
  if (!IsValid() || !other.IsValid() || start_.GetFileId() != other.start_.GetFileId()) {
    return false;
  }
  return start_.GetOffset() < other.end_.GetOffset() &&
         other.start_.GetOffset() < end_.GetOffset();
}

std::optional<SourceSpan> SourceSpan::Merge(const SourceSpan& other) const noexcept {
  if (!IsValid() || !other.IsValid() || start_.GetFileId() != other.start_.GetFileId()) {
    return std::nullopt;
  }
  const std::size_t merged_start = std::min(start_.GetOffset(), other.start_.GetOffset());
  const std::size_t merged_end = std::max(end_.GetOffset(), other.end_.GetOffset());
  SourceLocation start(start_.GetFileId(), merged_start, 1, 1);
  SourceLocation end(start_.GetFileId(), merged_end, 1, 1);
  return SourceSpan(start, end);
}

std::string SourceSpan::ToString() const {
  if (!IsValid()) {
    return "<invalid span>";
  }
  return std::format("file:{} [{}:{}-{}:{}] bytes:[{}-{})", start_.GetFileId().Value(),
                     start_.GetLine(), start_.GetColumn(), end_.GetLine(), end_.GetColumn(),
                     start_.GetOffset(), end_.GetOffset());
}

}  // namespace myc::source
