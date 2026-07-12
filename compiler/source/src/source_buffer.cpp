#include "myc/source/source_buffer.hpp"

#include <algorithm>

namespace myc::source {

namespace {

bool ValidateUtf8(std::string_view text) {
    const auto* bytes = reinterpret_cast<const unsigned char*>(text.data());
    std::size_t i = 0;
    while (i < text.size()) {
        const unsigned char byte = bytes[i];
        if (byte <= 0x7F) {
            ++i;
            continue;
        }
        std::size_t continuation = 0;
        if ((byte & 0xE0) == 0xC0) {
            continuation = 1;
        } else if ((byte & 0xF0) == 0xE0) {
            continuation = 2;
        } else if ((byte & 0xF8) == 0xF0) {
            continuation = 3;
        } else {
            return false;
        }
        if (i + continuation >= text.size()) {
            return false;
        }
        for (std::size_t j = 1; j <= continuation; ++j) {
            if ((bytes[i + j] & 0xC0) != 0x80) {
                return false;
            }
        }
        i += continuation + 1;
    }
    return true;
}

}  // namespace

SourceBuffer::SourceBuffer(FileId id, std::string path, std::string content)
    : id_(id), path_(std::move(path)), content_(std::move(content)) {
    valid_utf8_ = ValidateUtf8(content_);
    BuildLineIndex();
}

FileId SourceBuffer::GetId() const noexcept { return id_; }

const std::string& SourceBuffer::GetPath() const noexcept { return path_; }

std::string_view SourceBuffer::GetContent() const noexcept { return content_; }

std::size_t SourceBuffer::GetSize() const noexcept { return content_.size(); }

bool SourceBuffer::IsValidUtf8() const noexcept { return valid_utf8_; }

std::pair<LineNumber, ColumnNumber> SourceBuffer::GetLineAndColumn(
    std::size_t offset) const {
    if (line_starts_.empty() || offset > content_.size()) {
        return {1, 1};
    }
    const auto it = std::upper_bound(line_starts_.begin(), line_starts_.end(), offset);
    const std::size_t line_index =
        static_cast<std::size_t>(std::distance(line_starts_.begin(), it)) - 1;
    const LineNumber line = static_cast<LineNumber>(line_index + 1);
    const std::size_t line_start = line_starts_[line_index];
    const ColumnNumber column =
        static_cast<ColumnNumber>(offset - line_start + 1);
    return {line, column};
}

std::size_t SourceBuffer::GetLineStartOffset(LineNumber line) const {
    if (line == 0 || line > line_starts_.size()) {
        return 0;
    }
    return line_starts_[static_cast<std::size_t>(line - 1)];
}

LineNumber SourceBuffer::GetLineCount() const noexcept {
    return static_cast<LineNumber>(line_starts_.size());
}

void SourceBuffer::BuildLineIndex() {
    line_starts_.push_back(0);
    for (std::size_t i = 0; i < content_.size(); ++i) {
        if (content_[i] == '\n') {
            line_starts_.push_back(i + 1);
        }
    }
}

}  // namespace myc::source
