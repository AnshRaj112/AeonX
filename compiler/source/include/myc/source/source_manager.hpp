#pragma once

#include "myc/source/source_buffer.hpp"

#include <filesystem>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

namespace myc::source {

/// Manages a collection of source file buffers.
class SourceManager {
public:
    SourceManager();

    /// Loads a file from disk. Returns the assigned FileId.
    [[nodiscard]] std::optional<FileId> LoadFile(const std::filesystem::path& path);

    /// Registers in-memory source (for REPL or tests).
    [[nodiscard]] FileId AddMemoryBuffer(std::string name, std::string content);

    [[nodiscard]] const SourceBuffer* GetBuffer(FileId id) const;
    [[nodiscard]] std::string_view GetPath(FileId id) const;

    [[nodiscard]] std::size_t GetBufferCount() const noexcept;

private:
    FileId next_id_{1};
    std::vector<std::unique_ptr<SourceBuffer>> buffers_;
    std::unordered_map<std::string, FileId> path_index_;
};

}  // namespace myc::source
