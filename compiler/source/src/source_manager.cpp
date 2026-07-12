#include "myc/source/source_manager.hpp"

#include <fstream>
#include <sstream>

namespace myc::source {

SourceManager::SourceManager() = default;

std::optional<FileId> SourceManager::LoadFile(const std::filesystem::path& path) {
    const std::string path_string = path.string();
    const auto existing = path_index_.find(path_string);
    if (existing != path_index_.end()) {
        return existing->second;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return std::nullopt;
    }

    std::ostringstream stream;
    stream << file.rdbuf();
    const std::string content = stream.str();

    const FileId id = next_id_++;
    auto buffer = std::make_unique<SourceBuffer>(id, path_string, content);
    path_index_[path_string] = id;
    buffers_.push_back(std::move(buffer));
    return id;
}

FileId SourceManager::AddMemoryBuffer(std::string name, std::string content) {
    const FileId id = next_id_++;
    auto buffer = std::make_unique<SourceBuffer>(id, std::move(name), std::move(content));
    path_index_[buffer->GetPath()] = id;
    buffers_.push_back(std::move(buffer));
    return id;
}

const SourceBuffer* SourceManager::GetBuffer(FileId id) const {
    for (const auto& buffer : buffers_) {
        if (buffer->GetId() == id) {
            return buffer.get();
        }
    }
    return nullptr;
}

std::string_view SourceManager::GetPath(FileId id) const {
    if (const SourceBuffer* buffer = GetBuffer(id)) {
        return buffer->GetPath();
    }
    return {};
}

std::size_t SourceManager::GetBufferCount() const noexcept { return buffers_.size(); }

}  // namespace myc::source
