#include "myc/diagnostics/diagnostic_reporter.hpp"

#include "myc/diagnostics/diagnostic_json.hpp"

namespace myc::diagnostics {

DiagnosticReporter::DiagnosticReporter() : options_() {}

DiagnosticReporter::DiagnosticReporter(Options options) : options_(std::move(options)) {}

void DiagnosticReporter::SetOptions(Options options) { options_ = std::move(options); }

const DiagnosticReporter::Options& DiagnosticReporter::GetOptions() const noexcept {
    return options_;
}

void DiagnosticReporter::Report(const DiagnosticEngine& engine, std::ostream& stream) const {
    const auto& diagnostics = engine.GetDiagnostics();
    switch (options_.format) {
        case DiagnosticOutputFormat::PlainText: {
            DiagnosticFormatter formatter(FormatterOptions{
                .colors_enabled = options_.colors_enabled,
                .show_source_snippets = true,
                .show_notes = options_.show_notes,
                .show_hints = options_.show_hints,
                .source_manager = options_.source_manager,
            });
            stream << formatter.FormatAll(diagnostics);
            if (!diagnostics.empty()) {
                stream << '\n';
            }
            break;
        }
        case DiagnosticOutputFormat::Json:
            stream << DiagnosticJsonWriter::Write(diagnostics) << '\n';
            break;
        case DiagnosticOutputFormat::LanguageServerProtocol:
            stream << R"({"lsp":"not_implemented","message":"LSP output is reserved for future integration"})"
                   << '\n';
            break;
    }
}

}  // namespace myc::diagnostics
