#pragma once

#include "myc/diagnostics/diagnostic.hpp"

#include <functional>
#include <memory>
#include <ostream>
#include <vector>

namespace myc::diagnostics {

/// Collects, filters, and renders compiler diagnostics.
class DiagnosticEngine {
public:
    DiagnosticEngine();

    void Emit(std::unique_ptr<Diagnostic> diagnostic);
    void EmitError(DiagnosticCode code, std::string message);
    void EmitWarning(DiagnosticCode code, std::string message);
    void EmitNote(DiagnosticCode code, std::string message);
    void EmitHint(DiagnosticCode code, std::string message);

    [[nodiscard]] const std::vector<std::unique_ptr<Diagnostic>>& GetDiagnostics() const noexcept;
    [[nodiscard]] std::size_t GetErrorCount() const noexcept;
    [[nodiscard]] std::size_t GetWarningCount() const noexcept;
    [[nodiscard]] bool HasErrors() const noexcept;

    void Clear();

    /// Renders all diagnostics to the given output stream.
    void PrintAll(std::ostream& stream) const;

    /// Returns process exit code: 0 if no errors, 1 otherwise.
    [[nodiscard]] int GetExitCode() const noexcept;

private:
    std::vector<std::unique_ptr<Diagnostic>> diagnostics_;
    std::size_t error_count_{0};
    std::size_t warning_count_{0};
};

}  // namespace myc::diagnostics
