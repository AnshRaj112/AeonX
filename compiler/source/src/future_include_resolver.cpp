#include "myc/source/future_include_resolver.hpp"

namespace myc::source {

std::optional<std::filesystem::path> FutureIncludeResolver::ResolveInclude(
    std::string_view include_path, const SourceFile& parent) const {
  (void)include_path;
  (void)parent;
  // Placeholder for module, package, and #include resolution.
  return std::nullopt;
}

void FutureIncludeResolver::AddSearchPath(std::filesystem::path path) {
  search_paths_.push_back(std::move(path));
}

}  // namespace myc::source
