#include "myc/source/source_file.hpp"

#include "myc/source/encoding.hpp"
#include "myc/source/source_buffer.hpp"

namespace myc::source {

namespace {

std::string PathToString(const std::filesystem::path& path) {
  return path.generic_string();
}

}  // namespace

SourceFile::SourceFile(SourceFileID id, Metadata metadata, std::string content)
    : id_(id), metadata_(std::move(metadata)) {
  const std::size_t bom = EncodingDetector::BomLength(content);
  if (bom > 0) {
    content.erase(0, bom);
  }

  LineTable line_table(content);
  const bool valid_utf8 = metadata_.encoding == SourceEncoding::UTF8;
  buffer_ = std::make_unique<SourceBuffer>(id_, PathToString(metadata_.absolute_path),
                                           std::move(content), std::move(line_table),
                                           valid_utf8);
}

SourceFile::~SourceFile() = default;

SourceFileID SourceFile::GetId() const noexcept { return id_; }

const SourceFile::Metadata& SourceFile::GetMetadata() const noexcept { return metadata_; }

const SourceBuffer& SourceFile::GetBuffer() const noexcept { return *buffer_; }

const std::filesystem::path& SourceFile::GetAbsolutePath() const noexcept {
  return metadata_.absolute_path;
}

const std::filesystem::path& SourceFile::GetRelativePath() const noexcept {
  return metadata_.relative_path;
}

std::string_view SourceFile::GetFilename() const noexcept { return metadata_.filename; }

std::string_view SourceFile::GetExtension() const noexcept { return metadata_.extension; }

SourceEncoding SourceFile::GetEncoding() const noexcept { return metadata_.encoding; }

VirtualFileKind SourceFile::GetKind() const noexcept { return metadata_.virtual_info.kind; }

}  // namespace myc::source
