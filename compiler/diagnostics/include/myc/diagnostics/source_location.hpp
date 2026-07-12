#pragma once

#include "myc/types.hpp"

#include <string>

namespace myc::diagnostics {

/// A single point in source code with display and absolute path information.
struct SourceLocation {
    std::string filename;
    std::string absolute_path;
    LineNumber line{1};
    ColumnNumber column{1};

    [[nodiscard]] static SourceLocation Unknown() { return {}; }

    [[nodiscard]] bool HasFilename() const noexcept { return !filename.empty(); }

    [[nodiscard]] bool operator==(const SourceLocation& other) const = default;
};

}  // namespace myc::diagnostics
