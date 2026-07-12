#pragma once

#include "myc/diagnostics/diagnostic.hpp"
#include "myc/diagnostics/diagnostic_output.hpp"

#include <memory>
#include <string>
#include <vector>

namespace myc::source {
class SourceManager;
}

namespace myc::diagnostics {

/// Configuration for human-readable diagnostic rendering.
struct FormatterOptions {
    bool colors_enabled{false};
    bool show_source_snippets{true};
    bool show_notes{true};
    bool show_hints{true};
    const source::SourceManager* source_manager{nullptr};
};

/// Renders diagnostics into rustc/clang-style plain text output.
class DiagnosticFormatter {
public:
    explicit DiagnosticFormatter(FormatterOptions options = {});

    void SetOptions(FormatterOptions options);
    [[nodiscard]] const FormatterOptions& GetOptions() const noexcept;

    [[nodiscard]] std::string Format(const Diagnostic& diagnostic) const;
    [[nodiscard]] std::string FormatAll(
        const std::vector<std::unique_ptr<Diagnostic>>& diagnostics) const;

private:
    [[nodiscard]] std::string FormatPrimary(const Diagnostic& diagnostic) const;
    [[nodiscard]] std::string FormatSnippet(const Diagnostic& diagnostic) const;
    [[nodiscard]] std::string FormatSecondary(const Diagnostic& diagnostic) const;

    FormatterOptions options_;
};

}  // namespace myc::diagnostics
