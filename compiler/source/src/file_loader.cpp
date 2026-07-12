#include "myc/source/file_loader.hpp"

#include "myc/source/encoding.hpp"

#include <fstream>
#include <sstream>
#include <system_error>

namespace myc::source {

FileLoader::FileLoader(std::filesystem::path working_directory)
    : working_directory_(std::move(working_directory)) {}

void FileLoader::SetWorkingDirectory(std::filesystem::path path) {
  working_directory_ = std::move(path);
}

common::Result<LoadedFileData> FileLoader::Load(const std::filesystem::path& path) const {
  std::error_code ec;
  const std::filesystem::path absolute_path = NormalizePath(path);

  if (!std::filesystem::exists(absolute_path, ec)) {
    return std::unexpected("source file not found: " + absolute_path.string());
  }
  if (ec) {
    return std::unexpected("failed to access path: " + absolute_path.string() + " (" +
                           ec.message() + ")");
  }

  if (!std::filesystem::is_regular_file(absolute_path, ec)) {
    return std::unexpected("path is not a regular file: " + absolute_path.string());
  }

  std::ifstream file(absolute_path, std::ios::binary);
  if (!file) {
    return std::unexpected("failed to open source file: " + absolute_path.string());
  }

  std::ostringstream stream;
  stream << file.rdbuf();
  if (!file && !file.eof()) {
    return std::unexpected("failed to read source file: " + absolute_path.string());
  }

  LoadedFileData data;
  data.content = stream.str();
  data.absolute_path = absolute_path;
  data.file_size = data.content.size();

  const auto file_time = std::filesystem::last_write_time(absolute_path, ec);
  if (!ec) {
    data.last_modified = std::chrono::clock_cast<std::chrono::system_clock>(file_time);
  }

  std::error_code relative_ec;
  data.relative_path = std::filesystem::relative(absolute_path, working_directory_, relative_ec);
  if (relative_ec) {
    data.relative_path = absolute_path.filename();
  }

  data.encoding = EncodingDetector::Detect(data.content);
  if (data.encoding == SourceEncoding::Invalid) {
    return std::unexpected("invalid UTF-8 encoding in file: " + absolute_path.string());
  }

  return data;
}

std::filesystem::path FileLoader::ResolvePath(const std::filesystem::path& path) const {
  return NormalizePath(path);
}

std::filesystem::path FileLoader::NormalizePath(const std::filesystem::path& path) const {
  std::error_code ec;
  std::filesystem::path resolved = path;
  if (path.is_relative()) {
    resolved = working_directory_ / path;
  }

  std::filesystem::path absolute = std::filesystem::absolute(resolved, ec);
  if (ec) {
    absolute = resolved;
  }

  std::filesystem::path canonical = std::filesystem::weakly_canonical(absolute, ec);
  if (!ec) {
    return canonical;
  }
  return absolute;
}

}  // namespace myc::source
