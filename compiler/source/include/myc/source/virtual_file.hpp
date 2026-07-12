#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace myc::source {

/// Classification of source origins for future virtual and generated files.
enum class VirtualFileKind : std::uint8_t {
  Physical,
  Memory,
  Generated,
  ReplInput,
  IdeBuffer,
  MacroExpansion,
  ModuleImport,
  PackageImport,
};

[[nodiscard]] constexpr std::string_view ToString(VirtualFileKind kind) noexcept {
  switch (kind) {
    case VirtualFileKind::Physical:
      return "physical";
    case VirtualFileKind::Memory:
      return "memory";
    case VirtualFileKind::Generated:
      return "generated";
    case VirtualFileKind::ReplInput:
      return "repl";
    case VirtualFileKind::IdeBuffer:
      return "ide-buffer";
    case VirtualFileKind::MacroExpansion:
      return "macro-expansion";
    case VirtualFileKind::ModuleImport:
      return "module-import";
    case VirtualFileKind::PackageImport:
      return "package-import";
  }
  return "unknown";
}

/// Metadata describing a non-physical or overlay source file.
struct VirtualFile {
  VirtualFileKind kind{VirtualFileKind::Physical};
  std::string display_name;
  bool is_unsaved{false};

  // Future: memory-mapped backing, editor revision, macro expansion chain.
  std::uint64_t revision{0};
};

}  // namespace myc::source
