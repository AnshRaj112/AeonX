#pragma once

#include "myc/diagnostics/diagnostic.hpp"
#include "myc/diagnostics/diagnostic_engine.hpp"

namespace myc::diagnostics {

/// Fluent builder for constructing and emitting structured diagnostics.
class DiagnosticBuilder {
public:
    DiagnosticBuilder(DiagnosticEngine& engine, Severity severity);

    DiagnosticBuilder& Fatal(ErrorCode code, std::string message);
    DiagnosticBuilder& Error(ErrorCode code, std::string message);
    DiagnosticBuilder& Warning(ErrorCode code, std::string message);
    DiagnosticBuilder& Note(ErrorCode code, std::string message);
    DiagnosticBuilder& Hint(ErrorCode code, std::string message);
    DiagnosticBuilder& Info(ErrorCode code, std::string message);

    DiagnosticBuilder& At(SourceSpan span);
    DiagnosticBuilder& At(SourceLocation location);
    DiagnosticBuilder& Label(std::string text);

    DiagnosticBuilder& Note(std::string message);
    DiagnosticBuilder& Hint(std::string message);
    DiagnosticBuilder& FixIt(FixItHint hint);

    [[nodiscard]] bool Emit();

private:
    void SetPrimary(Severity severity, ErrorCode code, std::string message);

    DiagnosticEngine& engine_;
    Diagnostic diagnostic_;
    bool emitted_{false};
};

}  // namespace myc::diagnostics
