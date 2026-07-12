#include "myc/version/version.hpp"

#include <sstream>
#include <string>

namespace myc::version {

namespace {

constexpr std::string_view kGitCommitPlaceholder = "unknown";

}  // namespace

std::string_view GetGitCommit() { return kGitCommitPlaceholder; }

std::string_view GetBuildDate() {
#ifdef MYC_BUILD_DATE
    return MYC_BUILD_DATE;
#else
    return "unknown";
#endif
}

std::string GetFullVersionString() {
    std::ostringstream stream;
    stream << kLanguageName << " compiler " << kCompilerVersion << '\n'
           << "  Build date: " << GetBuildDate() << '\n'
           << "  Git commit: " << GetGitCommit() << '\n'
           << "  LLVM:       " << kLlvmVersion;
    return stream.str();
}

}  // namespace myc::version
