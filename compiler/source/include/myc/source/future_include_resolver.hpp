#pragma once

#include "myc/source/source_file.hpp"

#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

namespace myc::source {

/// Placeholder for future module, package, and include resolution.
class FutureIncludeResolver {
 public:
  FutureIncludeResolver() = default;

  /// Resolves an include/import path relative to a parent file.
  [[nodiscard]] std::optional<std::filesystem::path> ResolveInclude(
      std::string_view include_path, const SourceFile& parent) const;

  /// Registers a search path for future package imports.
  void AddSearchPath(std::filesystem::path path);

  [[nodiscard]] const std::vector<std::filesystem::path>& GetSearchPaths() const noexcept {
    return search_paths_;
  }

 private:
  std::vector<std::filesystem::path> search_paths_;
};

}  // namespace myc::source
