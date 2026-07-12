#include "myc/diagnostics/diagnostic_formatter.hpp"

#include "myc/diagnostics/ansi_color.hpp"
#include "myc/source/source_manager.hpp"

#include <format>
#include <sstream>
#include <string>

namespace myc::diagnostics {

namespace {

std::string ResolveDisplayPath(const Diagnostic& diagnostic,
                               const source::SourceManager* source_manager) {
    if (const auto& span = diagnostic.GetSpan(); span.has_value()) {
        if (span->start.HasFilename()) {
            return span->start.filename;
        }
        if (source_manager != nullptr && span->file_id.has_value()) {
            return std::string(source_manager->GetPath(*span->file_id));
        }
    }
    return {};
}

std::string ExtractLine(const source::SourceManager* source_manager, const FileId file_id,
                        const LineNumber line) {
    if (source_manager == nullptr) {
        return {};
    }
    const source::SourceBuffer* buffer = source_manager->GetBuffer(file_id);
    if (buffer == nullptr) {
        return {};
    }
    const std::size_t line_start = buffer->GetLineStartOffset(line);
    const std::string_view content = buffer->GetContent();
    if (line_start >= content.size()) {
        return {};
    }
    const std::size_t line_end = content.find('\n', line_start);
    if (line_end == std::string_view::npos) {
        return std::string(content.substr(line_start));
    }
    return std::string(content.substr(line_start, line_end - line_start));
}

}  // namespace

DiagnosticFormatter::DiagnosticFormatter(FormatterOptions options)
    : options_(std::move(options)) {}

void DiagnosticFormatter::SetOptions(FormatterOptions options) { options_ = std::move(options); }

const FormatterOptions& DiagnosticFormatter::GetOptions() const noexcept { return options_; }

std::string DiagnosticFormatter::Format(const Diagnostic& diagnostic) const {
    std::ostringstream output;
    output << FormatPrimary(diagnostic);
    if (options_.show_source_snippets) {
        const std::string snippet = FormatSnippet(diagnostic);
        if (!snippet.empty()) {
            output << '\n' << snippet;
        }
    }
    if (options_.show_notes) {
        for (const Diagnostic& note : diagnostic.GetNotes()) {
            output << '\n' << FormatSecondary(note);
        }
    }
    if (options_.show_hints) {
        for (const Diagnostic& hint : diagnostic.GetHints()) {
            output << '\n' << FormatSecondary(hint);
        }
    }
    return output.str();
}

std::string DiagnosticFormatter::FormatAll(
    const std::vector<std::unique_ptr<Diagnostic>>& diagnostics) const {
    std::ostringstream output;
    bool first = true;
    for (const auto& diagnostic : diagnostics) {
        if (!first) {
            output << "\n\n";
        }
        first = false;
        output << Format(*diagnostic);
    }
    return output.str();
}

std::string DiagnosticFormatter::FormatPrimary(const Diagnostic& diagnostic) const {
    const std::string severity =
        std::string(ansi::Colorize(ToString(diagnostic.GetSeverity()),
                                  ansi::SeverityColor(diagnostic.GetSeverity()),
                                  options_.colors_enabled));
    const std::string code = diagnostic.GetCode().GetCode().empty()
                                 ? ""
                                 : std::string("[") + diagnostic.GetCode().AsString() + "]";
    return std::format("{}: {}", severity + code, diagnostic.GetMessage());
}

std::string DiagnosticFormatter::FormatSnippet(const Diagnostic& diagnostic) const {
    const auto& span = diagnostic.GetSpan();
    if (!span.has_value()) {
        return {};
    }

    const std::string path = ResolveDisplayPath(diagnostic, options_.source_manager);
    if (path.empty() && !span->start.HasFilename()) {
        return {};
    }

    std::ostringstream output;
    const std::string arrow_target =
        path.empty() ? std::format("{}:{}", span->start.line, span->start.column)
                     : std::format("{}:{}:{}", path, span->start.line, span->start.column);
    output << ansi::Colorize(std::format("  --> {}", arrow_target), ansi::Blue,
                             options_.colors_enabled);

    std::string source_line;
    if (span->file_id.has_value()) {
        source_line = ExtractLine(options_.source_manager, *span->file_id, span->start.line);
    }

    if (!source_line.empty()) {
        output << "\n   |\n";
        output << std::format("{:>3} | {}", span->start.line, source_line) << '\n';
        output << "   | ";

        const std::size_t caret_pos =
            span->start.column > 0 ? static_cast<std::size_t>(span->start.column - 1) : 0;
        output << std::string(caret_pos, ' ');
        const std::size_t span_width =
            span->end_offset > span->start_offset
                ? span->end_offset - span->start_offset
                : 1;
        output << ansi::Colorize(std::string(span_width, '^'), ansi::Red, options_.colors_enabled);
        if (!diagnostic.GetLabel().empty()) {
            output << ' ' << diagnostic.GetLabel();
        }
    }

    return output.str();
}

std::string DiagnosticFormatter::FormatSecondary(const Diagnostic& diagnostic) const {
    const std::string severity =
        std::string(ansi::Colorize(ToString(diagnostic.GetSeverity()),
                                  ansi::SeverityColor(diagnostic.GetSeverity()),
                                  options_.colors_enabled));
    std::ostringstream output;
    output << severity << ":\n  " << diagnostic.GetMessage();
    return output.str();
}

}  // namespace myc::diagnostics
