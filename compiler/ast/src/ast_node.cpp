#include "myc/ast/ast_node.hpp"

namespace myc::ast {

std::optional<source::SourceSpan> MergeSpans(const source::SourceSpan& first,
                                             const source::SourceSpan& second) noexcept {
    if (!first.IsValid() || !second.IsValid()) {
        return std::nullopt;
    }
    return first.Merge(second);
}

source::SourceSpan ExtendSpan(const source::SourceSpan& base,
                              const source::SourceSpan& extra) noexcept {
    if (!base.IsValid()) {
        return extra;
    }
    if (!extra.IsValid()) {
        return base;
    }
    if (const auto merged = base.Merge(extra)) {
        return *merged;
    }
    return base;
}

}  // namespace myc::ast
