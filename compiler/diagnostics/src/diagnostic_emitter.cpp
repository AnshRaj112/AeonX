#include "myc/diagnostics/diagnostic_emitter.hpp"

#include <utility>

namespace myc::diagnostics {

DiagnosticEmitter::DiagnosticEmitter(DiagnosticEngine& engine) : engine_(engine) {}

DiagnosticBuilder DiagnosticEmitter::Builder(const Severity severity) {
    return DiagnosticBuilder(engine_, severity);
}

bool DiagnosticEmitter::EmitFatal(const ErrorCode code, std::string message) {
    return engine_.EmitFatal(code, std::move(message));
}

bool DiagnosticEmitter::EmitError(const ErrorCode code, std::string message) {
    return engine_.EmitError(code, std::move(message));
}

bool DiagnosticEmitter::EmitWarning(const ErrorCode code, std::string message) {
    return engine_.EmitWarning(code, std::move(message));
}

void DiagnosticEmitter::EmitInfo(const ErrorCode code, std::string message) {
    engine_.EmitInfo(code, std::move(message));
}

}  // namespace myc::diagnostics
