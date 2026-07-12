#include "myc/diagnostics/diagnostic_engine.hpp"

#include "myc/diagnostics/diagnostic_reporter.hpp"

#include <ostream>
#include <utility>

namespace myc::diagnostics {

DiagnosticEngine::DiagnosticEngine() = default;

void DiagnosticEngine::SetPolicy(const DiagnosticsPolicy policy) {
    const std::lock_guard lock(mutex_);
    policy_ = policy;
}

const DiagnosticsPolicy& DiagnosticEngine::GetPolicy() const noexcept { return policy_; }

void DiagnosticEngine::Emit(std::unique_ptr<Diagnostic> diagnostic) {
    if (!diagnostic) {
        return;
    }
    (void)Emit(*diagnostic);
}

bool DiagnosticEngine::Emit(const Diagnostic& diagnostic) {
    const std::lock_guard lock(mutex_);

    if (!ShouldAccept(diagnostic)) {
        return false;
    }

    if (policy_.max_diagnostics > 0 &&
        diagnostics_.size() >= policy_.max_diagnostics) {
        truncated_ = true;
        return false;
    }

    RecordCounts(diagnostic);
    diagnostics_.push_back(std::make_unique<Diagnostic>(diagnostic));
    return true;
}

bool DiagnosticEngine::EmitFatal(const ErrorCode code, std::string message) {
    return Emit(Diagnostic(Severity::Fatal, code, std::move(message)));
}

bool DiagnosticEngine::EmitError(const ErrorCode code, std::string message) {
    return Emit(Diagnostic(Severity::Error, code, std::move(message)));
}

bool DiagnosticEngine::EmitWarning(const ErrorCode code, std::string message) {
    return Emit(Diagnostic(Severity::Warning, code, std::move(message)));
}

void DiagnosticEngine::EmitNote(const ErrorCode code, std::string message) {
    (void)Emit(Diagnostic(Severity::Note, code, std::move(message)));
}

void DiagnosticEngine::EmitHint(const ErrorCode code, std::string message) {
    (void)Emit(Diagnostic(Severity::Hint, code, std::move(message)));
}

void DiagnosticEngine::EmitInfo(const ErrorCode code, std::string message) {
    (void)Emit(Diagnostic(Severity::Info, code, std::move(message)));
}

void DiagnosticEngine::EmitError(const std::string& code, std::string message) {
    (void)EmitError(ErrorCode(code), std::move(message));
}

void DiagnosticEngine::EmitWarning(const std::string& code, std::string message) {
    (void)EmitWarning(ErrorCode(code), std::move(message));
}

void DiagnosticEngine::EmitNote(const std::string& code, std::string message) {
    (void)EmitNote(ErrorCode(code), std::move(message));
}

void DiagnosticEngine::EmitHint(const std::string& code, std::string message) {
    (void)EmitHint(ErrorCode(code), std::move(message));
}

const std::vector<std::unique_ptr<Diagnostic>>& DiagnosticEngine::GetDiagnostics()
    const noexcept {
    return diagnostics_;
}

std::size_t DiagnosticEngine::GetFatalCount() const noexcept { return fatal_count_; }

std::size_t DiagnosticEngine::GetErrorCount() const noexcept { return error_count_; }

std::size_t DiagnosticEngine::GetWarningCount() const noexcept { return warning_count_; }

bool DiagnosticEngine::HasErrors() const noexcept {
    return fatal_count_ > 0 || error_count_ > 0;
}

bool DiagnosticEngine::HasFatals() const noexcept { return fatal_count_ > 0; }

bool DiagnosticEngine::ShouldContinue() const noexcept {
    if (!policy_.error_recovery) {
        return !HasErrors();
    }
    return !HasFatals();
}

bool DiagnosticEngine::WasTruncated() const noexcept { return truncated_; }

void DiagnosticEngine::Clear() {
    const std::lock_guard lock(mutex_);
    diagnostics_.clear();
    fatal_count_ = 0;
    error_count_ = 0;
    warning_count_ = 0;
    truncated_ = false;
}

void DiagnosticEngine::PrintAll(std::ostream& stream) const {
    DiagnosticReporter reporter;
    reporter.Report(*this, stream);
}

int DiagnosticEngine::GetExitCode() const noexcept { return HasErrors() ? 1 : 0; }

bool DiagnosticEngine::ShouldAccept(const Diagnostic& diagnostic) const {
    switch (diagnostic.GetSeverity()) {
        case Severity::Warning:
            if (policy_.suppress_warnings) {
                return false;
            }
            if (policy_.warning_limit > 0 && warning_count_ >= policy_.warning_limit) {
                return false;
            }
            break;
        case Severity::Error:
            if (policy_.error_limit > 0 && error_count_ >= policy_.error_limit) {
                return false;
            }
            break;
        case Severity::Fatal:
            if (policy_.error_limit > 0 && fatal_count_ + error_count_ >= policy_.error_limit) {
                return false;
            }
            break;
        case Severity::Note:
            if (!policy_.show_notes) {
                return false;
            }
            break;
        case Severity::Hint:
            if (!policy_.show_hints) {
                return false;
            }
            break;
        case Severity::Info:
            break;
    }
    return true;
}

void DiagnosticEngine::RecordCounts(const Diagnostic& diagnostic) {
    switch (diagnostic.GetSeverity()) {
        case Severity::Fatal:
            ++fatal_count_;
            break;
        case Severity::Error:
            ++error_count_;
            break;
        case Severity::Warning:
            ++warning_count_;
            if (policy_.warnings_as_errors) {
                ++error_count_;
            }
            break;
        case Severity::Note:
        case Severity::Hint:
        case Severity::Info:
            break;
    }
}

}  // namespace myc::diagnostics
