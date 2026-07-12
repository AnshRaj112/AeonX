#pragma once

#include <string_view>

namespace myc::version {

/// Placeholder language name for the Myc programming language.
inline constexpr std::string_view kLanguageName = "Myc";

/// Semantic compiler version (major.minor.patch).
inline constexpr std::string_view kCompilerVersion = "0.1.0";

/// LLVM backend version placeholder for future integration.
inline constexpr std::string_view kLlvmVersion = "not-linked";

/// Git commit hash placeholder; populated at build time when available.
std::string_view GetGitCommit();

/// Build date string (UTC) generated at compile time.
std::string_view GetBuildDate();

/// Full version string suitable for `--version` output.
std::string GetFullVersionString();

}  // namespace myc::version
