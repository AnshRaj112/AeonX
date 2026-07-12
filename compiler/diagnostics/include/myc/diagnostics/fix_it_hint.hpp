#pragma once

#include "myc/diagnostics/source_span.hpp"

#include <string>

namespace myc::diagnostics {

/// A suggested source edit attached to a diagnostic.
struct FixItHint {
    SourceSpan range{};
    std::string replacement;
    std::string description;
};

}  // namespace myc::diagnostics
