#pragma once

#include "myc/diagnostics/diagnostic.hpp"

#include <cstdint>
#include <cstddef>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <vector>

namespace myc::diagnostics {

/// Controls diagnostic collection, filtering, and limits.
struct DiagnosticsPolicy {
    bool warnings_as_errors{false};
    bool suppress_warnings{false};
    bool show_notes{true};
    bool show_hints{true};
    bool error_recovery{true};
    std::uint32_t max_diagnostics{0};
    std::uint32_t error_limit{0};
    std::uint32_t warning_limit{0};
};

/// Collects diagnostics from all compiler phases without performing I/O.
class DiagnosticEngine {
public:
    DiagnosticEngine();

    void SetPolicy(DiagnosticsPolicy policy);
    [[nodiscard]] const DiagnosticsPolicy& GetPolicy() const noexcept;

    void Emit(std::unique_ptr<Diagnostic> diagnostic);
    [[nodiscard]] bool Emit(const Diagnostic& diagnostic);

    [[nodiscard]] bool EmitFatal(ErrorCode code, std::string message);
    [[nodiscard]] bool EmitError(ErrorCode code, std::string message);
    [[nodiscard]] bool EmitWarning(ErrorCode code, std::string message);
    void EmitNote(ErrorCode code, std::string message);
    void EmitHint(ErrorCode code, std::string message);
    void EmitInfo(ErrorCode code, std::string message);

    /// Legacy string-code overloads for existing call sites.
    void EmitError(const std::string& code, std::string message);
    void EmitWarning(const std::string& code, std::string message);
    void EmitNote(const std::string& code, std::string message);
    void EmitHint(const std::string& code, std::string message);

    [[nodiscard]] const std::vector<std::unique_ptr<Diagnostic>>& GetDiagnostics() const noexcept;
    [[nodiscard]] std::size_t GetFatalCount() const noexcept;
    [[nodiscard]] std::size_t GetErrorCount() const noexcept;
    [[nodiscard]] std::size_t GetWarningCount() const noexcept;
    [[nodiscard]] bool HasErrors() const noexcept;
    [[nodiscard]] bool HasFatals() const noexcept;
    [[nodiscard]] bool ShouldContinue() const noexcept;
    [[nodiscard]] bool WasTruncated() const noexcept;

    void Clear();

    /// Renders all diagnostics using the default plain-text reporter.
    void PrintAll(std::ostream& stream) const;

    [[nodiscard]] int GetExitCode() const noexcept;

private:
    [[nodiscard]] bool ShouldAccept(const Diagnostic& diagnostic) const;
    void RecordCounts(const Diagnostic& diagnostic);

    mutable std::mutex mutex_;
    DiagnosticsPolicy policy_;
    std::vector<std::unique_ptr<Diagnostic>> diagnostics_;
    std::size_t fatal_count_{0};
    std::size_t error_count_{0};
    std::size_t warning_count_{0};
    bool truncated_{false};
};

}  // namespace myc::diagnostics
