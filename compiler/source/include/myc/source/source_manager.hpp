#pragma once

#include "myc/source/file_loader.hpp"
#include "myc/source/future_include_resolver.hpp"
#include "myc/source/source_file.hpp"
#include "myc/source/source_location.hpp"
#include "myc/source/source_span.hpp"

#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace myc::diagnostics {
class DiagnosticEngine;
}

namespace myc::source {

/// Central registry for loaded source files.
class SourceManager {
 public:
  SourceManager();
  explicit SourceManager(std::filesystem::path working_directory);

  void SetDiagnosticsEngine(diagnostics::DiagnosticEngine* engine) noexcept;
  void SetWorkingDirectory(std::filesystem::path path);

  /// Loads a file from disk. Returns the assigned file ID, or nullopt on failure.
  [[nodiscard]] std::optional<FileId> LoadFile(const std::filesystem::path& path);

  /// Registers in-memory source (REPL, tests, IDE buffers).
  [[nodiscard]] FileId AddMemoryBuffer(std::string name, std::string content);

  /// Registers a virtual file with explicit metadata.
  [[nodiscard]] FileId AddVirtualFile(VirtualFile virtual_info, std::string content);

  [[nodiscard]] const SourceFile* GetFile(FileId id) const;
  [[nodiscard]] const SourceBuffer* GetBuffer(FileId id) const;
  [[nodiscard]] std::string_view GetPath(FileId id) const;

  [[nodiscard]] SourceLocation GetLocation(FileId id, std::size_t offset) const;
  [[nodiscard]] SourceSpan MakeSpan(FileId id, std::size_t start_offset,
                                    std::size_t end_offset) const;

  [[nodiscard]] std::size_t GetFileCount() const noexcept;
  [[nodiscard]] std::size_t GetBufferCount() const noexcept;

  [[nodiscard]] bool IsLoaded(const std::filesystem::path& path) const;
  [[nodiscard]] std::optional<FileId> FindFileByPath(
      const std::filesystem::path& path) const;

  [[nodiscard]] const FutureIncludeResolver& GetIncludeResolver() const noexcept;
  [[nodiscard]] FutureIncludeResolver& GetIncludeResolver() noexcept;

 private:
  [[nodiscard]] std::string MakeCacheKey(const std::filesystem::path& path) const;

  void EmitLoadError(const std::filesystem::path& path, std::string_view message) const;
  void EmitEncodingError(const std::filesystem::path& path) const;

  mutable std::shared_mutex mutex_;
  FileId next_id_{1};
  std::vector<std::unique_ptr<SourceFile>> files_;
  std::unordered_map<std::string, FileId> path_index_;
  FileLoader loader_;
  FutureIncludeResolver include_resolver_;
  diagnostics::DiagnosticEngine* diagnostics_{nullptr};
};

}  // namespace myc::source
