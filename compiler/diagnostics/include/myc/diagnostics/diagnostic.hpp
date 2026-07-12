#pragma once

#include "myc/types.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace myc::diagnostics {

/// Unique diagnostic error code (e.g., "E0001").
using DiagnosticCode = std::string;

/// A suggested source edit attached to a diagnostic.
struct FixItHint {
    SourceRange range{};
    std::string replacement;
    std::string description;
};

/// Base class for all compiler diagnostics.
class Diagnostic {
public:
    Diagnostic(Severity severity, DiagnosticCode code, std::string message);

    [[nodiscard]] Severity GetSeverity() const noexcept;
    [[nodiscard]] const DiagnosticCode& GetCode() const noexcept;
    [[nodiscard]] const std::string& GetMessage() const noexcept;

    void SetLocation(FileId file, LineNumber line, ColumnNumber column);
    [[nodiscard]] std::optional<FileId> GetFileId() const noexcept;
    [[nodiscard]] std::optional<LineNumber> GetLine() const noexcept;
    [[nodiscard]] std::optional<ColumnNumber> GetColumn() const noexcept;

    void AddFixIt(FixItHint hint);
    [[nodiscard]] const std::vector<FixItHint>& GetFixIts() const noexcept;

    [[nodiscard]] virtual std::string Format() const;

    virtual ~Diagnostic() = default;

protected:
    Severity severity_;
    DiagnosticCode code_;
    std::string message_;
    std::optional<FileId> file_id_;
    std::optional<LineNumber> line_;
    std::optional<ColumnNumber> column_;
    std::vector<FixItHint> fix_its_;
};

/// A fatal or recoverable error diagnostic.
class Error final : public Diagnostic {
public:
    Error(DiagnosticCode code, std::string message);
};

/// A warning diagnostic.
class Warning final : public Diagnostic {
public:
    Warning(DiagnosticCode code, std::string message);
};

/// Supplementary information attached to another diagnostic.
class Note final : public Diagnostic {
public:
    Note(DiagnosticCode code, std::string message);
};

/// A suggestion for resolving an issue.
class Hint final : public Diagnostic {
public:
    Hint(DiagnosticCode code, std::string message);
};

}  // namespace myc::diagnostics
