#pragma once

#include "myc/diagnostics/diagnostic.hpp"

#include <memory>
#include <string>
#include <vector>

namespace myc::diagnostics {

/// Serializes diagnostics to JSON for machine-readable tooling output.
class DiagnosticJsonWriter {
public:
    [[nodiscard]] static std::string Write(
        const std::vector<std::unique_ptr<Diagnostic>>& diagnostics);

    [[nodiscard]] static std::string Escape(std::string_view text);
};

}  // namespace myc::diagnostics
