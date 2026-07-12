#pragma once

#include "myc/diagnostics/diagnostic_builder.hpp"
#include "myc/diagnostics/diagnostic_engine.hpp"

namespace myc::diagnostics {

/// Phase-facing entry point for emitting diagnostics through the shared engine.
class DiagnosticEmitter {
public:
    explicit DiagnosticEmitter(DiagnosticEngine& engine);

    [[nodiscard]] DiagnosticBuilder Builder(Severity severity = Severity::Error);

    [[nodiscard]] bool EmitFatal(ErrorCode code, std::string message);
    [[nodiscard]] bool EmitError(ErrorCode code, std::string message);
    [[nodiscard]] bool EmitWarning(ErrorCode code, std::string message);
    void EmitInfo(ErrorCode code, std::string message);

private:
    DiagnosticEngine& engine_;
};

}  // namespace myc::diagnostics
