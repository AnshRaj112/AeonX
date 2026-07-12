#include "myc/source/source_manager.hpp"

#include "myc/diagnostics/diagnostic_engine.hpp"
#include "myc/diagnostics/error_code.hpp"
#include "myc/source/encoding.hpp"
#include "myc/source/source_buffer.hpp"

#include <utility>

namespace myc::source {

namespace {

SourceFile::Metadata BuildMetadata(const LoadedFileData& data, VirtualFile virtual_info) {
  SourceFile::Metadata metadata;
  metadata.absolute_path = data.absolute_path;
  metadata.relative_path = data.relative_path;
  metadata.filename = data.absolute_path.filename().string();
  metadata.extension = data.absolute_path.extension().string();
  metadata.last_modified = data.last_modified;
  metadata.file_size = data.file_size;
  metadata.encoding = data.encoding;
  metadata.virtual_info = std::move(virtual_info);
  return metadata;
}

SourceFile::Metadata BuildMemoryMetadata(std::string name, VirtualFile virtual_info) {
  SourceFile::Metadata metadata;
  metadata.absolute_path = name;
  metadata.relative_path = std::move(name);
  metadata.filename = metadata.absolute_path.filename().string();
  metadata.extension = metadata.absolute_path.extension().string();
  metadata.encoding = SourceEncoding::UTF8;
  metadata.virtual_info = std::move(virtual_info);
  return metadata;
}

}  // namespace

SourceManager::SourceManager() : loader_(std::filesystem::current_path()) {}

SourceManager::SourceManager(std::filesystem::path working_directory)
    : loader_(std::move(working_directory)) {}

void SourceManager::SetDiagnosticsEngine(diagnostics::DiagnosticEngine* engine) noexcept {
  diagnostics_ = engine;
}

void SourceManager::SetWorkingDirectory(std::filesystem::path path) {
  std::unique_lock lock(mutex_);
  loader_.SetWorkingDirectory(std::move(path));
}

std::optional<FileId> SourceManager::LoadFile(const std::filesystem::path& path) {
  const std::filesystem::path resolved_path = loader_.ResolvePath(path);
  const std::string cache_key = resolved_path.string();

  {
    std::shared_lock read_lock(mutex_);
    const auto existing = path_index_.find(cache_key);
    if (existing != path_index_.end()) {
      return existing->second;
    }
  }

  const auto loaded = loader_.Load(path);
  if (!loaded) {
    EmitLoadError(path, loaded.error());
    return std::nullopt;
  }

  VirtualFile virtual_info;
  virtual_info.kind = VirtualFileKind::Physical;
  virtual_info.display_name = loaded->absolute_path.filename().string();

  const FileId id = next_id_++;
  auto file = std::make_unique<SourceFile>(
      SourceFileID(id), BuildMetadata(*loaded, std::move(virtual_info)),
      std::move(loaded->content));

  std::unique_lock write_lock(mutex_);
  const auto existing = path_index_.find(cache_key);
  if (existing != path_index_.end()) {
    return existing->second;
  }

  path_index_[cache_key] = id;
  files_.push_back(std::move(file));
  return id;
}

FileId SourceManager::AddMemoryBuffer(std::string name, std::string content) {
  VirtualFile virtual_info;
  virtual_info.kind = VirtualFileKind::Memory;
  virtual_info.display_name = name;

  if (EncodingDetector::Detect(content) == SourceEncoding::Invalid) {
    EmitEncodingError(name);
    content.clear();
  }

  auto metadata = BuildMemoryMetadata(name, std::move(virtual_info));
  metadata.file_size = content.size();
  metadata.encoding = SourceEncoding::UTF8;

  const FileId id = next_id_++;
  auto file = std::make_unique<SourceFile>(SourceFileID(id), std::move(metadata),
                                           std::move(content));

  std::unique_lock lock(mutex_);
  path_index_[file->GetAbsolutePath().string()] = id;
  files_.push_back(std::move(file));
  return id;
}

FileId SourceManager::AddVirtualFile(VirtualFile virtual_info, std::string content) {
  const std::string name = virtual_info.display_name.empty() ? "<virtual>"
                                                             : virtual_info.display_name;

  if (EncodingDetector::Detect(content) == SourceEncoding::Invalid) {
    EmitEncodingError(name);
    content.clear();
  }

  auto metadata = BuildMemoryMetadata(name, std::move(virtual_info));
  metadata.file_size = content.size();
  metadata.encoding = SourceEncoding::UTF8;

  const FileId id = next_id_++;
  auto file = std::make_unique<SourceFile>(SourceFileID(id), std::move(metadata),
                                           std::move(content));

  std::unique_lock lock(mutex_);
  const std::string cache_key = file->GetAbsolutePath().string();
  path_index_[cache_key] = id;
  files_.push_back(std::move(file));
  return id;
}

const SourceFile* SourceManager::GetFile(FileId id) const {
  std::shared_lock lock(mutex_);
  for (const auto& file : files_) {
    if (file->GetId().Value() == id) {
      return file.get();
    }
  }
  return nullptr;
}

const SourceBuffer* SourceManager::GetBuffer(FileId id) const {
  if (const SourceFile* file = GetFile(id)) {
    return &file->GetBuffer();
  }
  return nullptr;
}

std::string_view SourceManager::GetPath(FileId id) const {
  if (const SourceFile* file = GetFile(id)) {
    return file->GetBuffer().GetPath();
  }
  return {};
}

SourceLocation SourceManager::GetLocation(FileId id, std::size_t offset) const {
  if (const SourceBuffer* buffer = GetBuffer(id)) {
    const auto [line, column] = buffer->GetLineAndColumn(offset);
    return SourceLocation(SourceFileID(id), offset, line, column);
  }
  return {};
}

SourceSpan SourceManager::MakeSpan(FileId id, std::size_t start_offset,
                                   std::size_t end_offset) const {
  return SourceSpan(GetLocation(id, start_offset), GetLocation(id, end_offset));
}

std::size_t SourceManager::GetFileCount() const noexcept {
  std::shared_lock lock(mutex_);
  return files_.size();
}

std::size_t SourceManager::GetBufferCount() const noexcept { return GetFileCount(); }

bool SourceManager::IsLoaded(const std::filesystem::path& path) const {
  return FindFileByPath(path).has_value();
}

std::optional<FileId> SourceManager::FindFileByPath(const std::filesystem::path& path) const {
  std::shared_lock lock(mutex_);
  const std::string cache_key = loader_.ResolvePath(path).string();
  const auto it = path_index_.find(cache_key);
  if (it == path_index_.end()) {
    return std::nullopt;
  }
  return it->second;
}

const FutureIncludeResolver& SourceManager::GetIncludeResolver() const noexcept {
  return include_resolver_;
}

FutureIncludeResolver& SourceManager::GetIncludeResolver() noexcept { return include_resolver_; }

std::string SourceManager::MakeCacheKey(const std::filesystem::path& path) const {
  return loader_.ResolvePath(path).string();
}

void SourceManager::EmitLoadError(const std::filesystem::path& path,
                                  std::string_view message) const {
  if (diagnostics_ == nullptr) {
    return;
  }
  const std::string text(message);
  if (text.find("not found") != std::string::npos) {
    (void)diagnostics_->EmitError(diagnostics::codes::SourceFileNotFound,
                                  std::string(path.string()) + ": " + text);
    return;
  }
  if (text.find("UTF-8") != std::string::npos) {
    (void)diagnostics_->EmitError(diagnostics::codes::SourceInvalidEncoding,
                                  std::string(path.string()) + ": " + text);
    return;
  }
  (void)diagnostics_->EmitError(diagnostics::codes::SourceFileReadError,
                                std::string(path.string()) + ": " + text);
}

void SourceManager::EmitEncodingError(const std::filesystem::path& path) const {
  if (diagnostics_ != nullptr) {
    (void)diagnostics_->EmitError(diagnostics::codes::SourceInvalidEncoding,
                                  "invalid UTF-8 encoding in file: " + path.string());
  }
}

}  // namespace myc::source
