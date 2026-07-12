#pragma once

#include "myc/types.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace myc::source {

/// Immutable in-memory representation of a single source file.
class SourceBuffer {
public:
    SourceBuffer(FileId id, std::string path, std::string content);

    [[nodiscard]] FileId GetId() const noexcept;
    [[nodiscard]] const std::string& GetPath() const noexcept;
    [[nodiscard]] std::string_view GetContent() const noexcept;
    [[nodiscard]] std::size_t GetSize() const noexcept;

    /// Returns true if content is valid UTF-8.
    [[nodiscard]] bool IsValidUtf8() const noexcept;

    /// Maps a byte offset to a 1-based line and column.
    [[nodiscard]] std::pair<LineNumber, ColumnNumber> GetLineAndColumn(
        std::size_t offset) const;

    /// Returns the byte offset of the start of the given 1-based line.
    [[nodiscard]] std::size_t GetLineStartOffset(LineNumber line) const;

    [[nodiscard]] LineNumber GetLineCount() const noexcept;

private:
    void BuildLineIndex();

    FileId id_;
    std::string path_;
    std::string content_;
    std::vector<std::size_t> line_starts_;
    bool valid_utf8_{true};
};

}  // namespace myc::source
