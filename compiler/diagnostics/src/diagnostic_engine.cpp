#include "myc/diagnostics/diagnostic_engine.hpp"

namespace myc::diagnostics {

DiagnosticEngine::DiagnosticEngine() = default;

void DiagnosticEngine::Emit(std::unique_ptr<Diagnostic> diagnostic) {
    if (!diagnostic) {
        return;
    }
    switch (diagnostic->GetSeverity()) {
        case Severity::Error:
            ++error_count_;
            break;
        case Severity::Warning:
            ++warning_count_;
            break;
        case Severity::Note:
        case Severity::Hint:
            break;
    }
    diagnostics_.push_back(std::move(diagnostic));
}

void DiagnosticEngine::EmitError(DiagnosticCode code, std::string message) {
    Emit(std::make_unique<Error>(std::move(code), std::move(message)));
}

void DiagnosticEngine::EmitWarning(DiagnosticCode code, std::string message) {
    Emit(std::make_unique<Warning>(std::move(code), std::move(message)));
}

void DiagnosticEngine::EmitNote(DiagnosticCode code, std::string message) {
    Emit(std::make_unique<Note>(std::move(code), std::move(message)));
}

void DiagnosticEngine::EmitHint(DiagnosticCode code, std::string message) {
    Emit(std::make_unique<Hint>(std::move(code), std::move(message)));
}

const std::vector<std::unique_ptr<Diagnostic>>& DiagnosticEngine::GetDiagnostics() const noexcept {
    return diagnostics_;
}

std::size_t DiagnosticEngine::GetErrorCount() const noexcept { return error_count_; }

std::size_t DiagnosticEngine::GetWarningCount() const noexcept { return warning_count_; }

bool DiagnosticEngine::HasErrors() const noexcept { return error_count_ > 0; }

void DiagnosticEngine::Clear() {
    diagnostics_.clear();
    error_count_ = 0;
    warning_count_ = 0;
}

void DiagnosticEngine::PrintAll(std::ostream& stream) const {
    for (const auto& diagnostic : diagnostics_) {
        stream << diagnostic->Format() << '\n';
    }
}

int DiagnosticEngine::GetExitCode() const noexcept { return HasErrors() ? 1 : 0; }

}  // namespace myc::diagnostics
