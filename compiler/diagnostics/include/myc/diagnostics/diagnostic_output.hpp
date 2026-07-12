#pragma once

#include <cstdint>

namespace myc::diagnostics {

/// Supported diagnostic output formats.
enum class DiagnosticOutputFormat : std::uint8_t {
    PlainText,
    Json,
    /// Reserved for future Language Server Protocol publishDiagnostics integration.
    LanguageServerProtocol,
};

}  // namespace myc::diagnostics
