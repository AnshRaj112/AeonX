#pragma once

#include "myc/common/result.hpp"
#include "myc/source/encoding.hpp"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>

namespace myc::source {

/// Result of reading a file from disk.
struct LoadedFileData {
  std::string content;
  std::filesystem::path absolute_path;
  std::filesystem::path relative_path;
  std::uintmax_t file_size{0};
  std::chrono::system_clock::time_point last_modified{};
  SourceEncoding encoding{SourceEncoding::UTF8};
};

/// Loads source files from disk with encoding validation.
class FileLoader {
 public:
  explicit FileLoader(std::filesystem::path working_directory = std::filesystem::current_path());

  void SetWorkingDirectory(std::filesystem::path path);

  /// Resolves a path to an absolute, canonical form for cache keys.
  [[nodiscard]] std::filesystem::path ResolvePath(
      const std::filesystem::path& path) const;

  /// Reads a file from disk. Returns an error message on failure.
  [[nodiscard]] common::Result<LoadedFileData> Load(
      const std::filesystem::path& path) const;

 private:
  [[nodiscard]] std::filesystem::path NormalizePath(
      const std::filesystem::path& path) const;

  std::filesystem::path working_directory_;
};

}  // namespace myc::source
