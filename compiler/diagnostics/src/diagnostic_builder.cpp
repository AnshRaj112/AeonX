#include "myc/diagnostics/diagnostic_builder.hpp"

#include <utility>

namespace myc::diagnostics {

DiagnosticBuilder::DiagnosticBuilder(DiagnosticEngine& engine, const Severity severity)
    : engine_(engine),
      diagnostic_(severity, ErrorCode(""), "") {}

DiagnosticBuilder& DiagnosticBuilder::Fatal(const ErrorCode code, std::string message) {
    SetPrimary(Severity::Fatal, code, std::move(message));
    return *this;
}

DiagnosticBuilder& DiagnosticBuilder::Error(const ErrorCode code, std::string message) {
    SetPrimary(Severity::Error, code, std::move(message));
    return *this;
}

DiagnosticBuilder& DiagnosticBuilder::Warning(const ErrorCode code, std::string message) {
    SetPrimary(Severity::Warning, code, std::move(message));
    return *this;
}

DiagnosticBuilder& DiagnosticBuilder::Note(const ErrorCode code, std::string message) {
    diagnostic_.AddNote(Diagnostic(Severity::Note, code, std::move(message)));
    return *this;
}

DiagnosticBuilder& DiagnosticBuilder::Hint(const ErrorCode code, std::string message) {
    diagnostic_.AddHint(Diagnostic(Severity::Hint, code, std::move(message)));
    return *this;
}

DiagnosticBuilder& DiagnosticBuilder::Info(const ErrorCode code, std::string message) {
    SetPrimary(Severity::Info, code, std::move(message));
    return *this;
}

DiagnosticBuilder& DiagnosticBuilder::At(const SourceSpan span) {
    diagnostic_.SetSpan(span);
    return *this;
}

DiagnosticBuilder& DiagnosticBuilder::At(const SourceLocation location) {
    SourceSpan span;
    span.start = location;
    span.end = location;
    diagnostic_.SetSpan(span);
    return *this;
}

DiagnosticBuilder& DiagnosticBuilder::Label(std::string text) {
    diagnostic_.SetLabel(std::move(text));
    return *this;
}

DiagnosticBuilder& DiagnosticBuilder::Note(std::string message) {
    diagnostic_.AddNote(Diagnostic(Severity::Note, ErrorCode(""), std::move(message)));
    return *this;
}

DiagnosticBuilder& DiagnosticBuilder::Hint(std::string message) {
    diagnostic_.AddHint(Diagnostic(Severity::Hint, ErrorCode(""), std::move(message)));
    return *this;
}

DiagnosticBuilder& DiagnosticBuilder::FixIt(FixItHint hint) {
    diagnostic_.AddFixIt(std::move(hint));
    return *this;
}

bool DiagnosticBuilder::Emit() {
    if (emitted_) {
        return false;
    }
    emitted_ = true;
    return engine_.Emit(diagnostic_);
}

void DiagnosticBuilder::SetPrimary(const Severity severity, const ErrorCode code,
                                   std::string message) {
    diagnostic_ = Diagnostic(severity, code, std::move(message));
}

}  // namespace myc::diagnostics
