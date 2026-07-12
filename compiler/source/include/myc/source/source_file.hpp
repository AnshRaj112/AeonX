#pragma once

#include "myc/source/encoding.hpp"
#include "myc/source/line_table.hpp"
#include "myc/source/source_buffer.hpp"
#include "myc/source/source_file_id.hpp"
#include "myc/source/virtual_file.hpp"

#include <chrono>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace myc::source {

class SourceBuffer;

/// Immutable metadata and text for a single loaded source file.
class SourceFile {
 public:
  struct Metadata {
    std::filesystem::path absolute_path;
    std::filesystem::path relative_path;
    std::string filename;
    std::string extension;
    std::chrono::system_clock::time_point last_modified{};
    std::uintmax_t file_size{0};
    SourceEncoding encoding{SourceEncoding::UTF8};
    VirtualFile virtual_info{};
    std::optional<std::string> content_hash;     // future
    std::optional<std::string> content_checksum;  // future
  };

  SourceFile(SourceFileID id, Metadata metadata, std::string content);
  ~SourceFile();

  [[nodiscard]] SourceFileID GetId() const noexcept;
  [[nodiscard]] const Metadata& GetMetadata() const noexcept;
  [[nodiscard]] const SourceBuffer& GetBuffer() const noexcept;

  [[nodiscard]] const std::filesystem::path& GetAbsolutePath() const noexcept;
  [[nodiscard]] const std::filesystem::path& GetRelativePath() const noexcept;
  [[nodiscard]] std::string_view GetFilename() const noexcept;
  [[nodiscard]] std::string_view GetExtension() const noexcept;
  [[nodiscard]] SourceEncoding GetEncoding() const noexcept;
  [[nodiscard]] VirtualFileKind GetKind() const noexcept;

 private:
  SourceFileID id_;
  Metadata metadata_;
  std::unique_ptr<SourceBuffer> buffer_;
};

}  // namespace myc::source
