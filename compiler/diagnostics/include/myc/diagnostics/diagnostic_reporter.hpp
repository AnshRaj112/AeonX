#pragma once

#include "myc/diagnostics/diagnostic_engine.hpp"
#include "myc/diagnostics/diagnostic_formatter.hpp"
#include "myc/diagnostics/diagnostic_output.hpp"

#include <ostream>

namespace myc::diagnostics {

/// Future extension point for Language Server Protocol serialization.
class DiagnosticLspSerializer {
public:
    virtual ~DiagnosticLspSerializer() = default;

    [[nodiscard]] virtual std::string Serialize(
        const std::vector<std::unique_ptr<Diagnostic>>& diagnostics) const = 0;
};

/// Routes collected diagnostics to the configured output format.
class DiagnosticReporter {
public:
    struct Options {
        DiagnosticOutputFormat format{DiagnosticOutputFormat::PlainText};
        bool colors_enabled{false};
        bool show_notes{true};
        bool show_hints{true};
        const source::SourceManager* source_manager{nullptr};
    };

    DiagnosticReporter();
    explicit DiagnosticReporter(Options options);

    void SetOptions(Options options);
    [[nodiscard]] const Options& GetOptions() const noexcept;

    void Report(const DiagnosticEngine& engine, std::ostream& stream) const;

private:
    Options options_;
};

}  // namespace myc::diagnostics
