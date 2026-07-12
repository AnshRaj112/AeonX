#pragma once

#include "myc/diagnostics/error_code.hpp"
#include "myc/diagnostics/fix_it_hint.hpp"
#include "myc/diagnostics/severity.hpp"
#include "myc/diagnostics/source_span.hpp"
#include "myc/types.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace myc::diagnostics {

/// A single compiler diagnostic with optional related notes and hints.
class Diagnostic {
public:
    Diagnostic(Severity severity, ErrorCode code, std::string message);

    [[nodiscard]] Severity GetSeverity() const noexcept;
    [[nodiscard]] const ErrorCode& GetCode() const noexcept;
    [[nodiscard]] const std::string& GetMessage() const noexcept;
    [[nodiscard]] const std::optional<SourceSpan>& GetSpan() const noexcept;
    [[nodiscard]] const std::string& GetLabel() const noexcept;
    [[nodiscard]] const std::vector<Diagnostic>& GetNotes() const noexcept;
    [[nodiscard]] const std::vector<Diagnostic>& GetHints() const noexcept;
    [[nodiscard]] const std::vector<FixItHint>& GetFixIts() const noexcept;

    void SetSpan(SourceSpan span);
    void SetLabel(std::string label);
    void AddNote(Diagnostic note);
    void AddHint(Diagnostic hint);
    void AddFixIt(FixItHint hint);

    /// Legacy location API retained for incremental migration.
    void SetLocation(FileId file, LineNumber line, ColumnNumber column);
    [[nodiscard]] std::optional<FileId> GetFileId() const noexcept;
    [[nodiscard]] std::optional<LineNumber> GetLine() const noexcept;
    [[nodiscard]] std::optional<ColumnNumber> GetColumn() const noexcept;

    [[nodiscard]] std::string Format() const;

private:
    Severity severity_;
    ErrorCode code_;
    std::string message_;
    std::optional<SourceSpan> span_;
    std::string label_;
    std::vector<Diagnostic> notes_;
    std::vector<Diagnostic> hints_;
    std::vector<FixItHint> fix_its_;
};

/// Convenience aliases for strongly typed construction.
class Error final : public Diagnostic {
public:
    Error(ErrorCode code, std::string message);
    Error(std::string_view code, std::string message);
};

class Warning final : public Diagnostic {
public:
    Warning(ErrorCode code, std::string message);
    Warning(std::string_view code, std::string message);
};

class Note final : public Diagnostic {
public:
    Note(ErrorCode code, std::string message);
    Note(std::string_view code, std::string message);
};

class Hint final : public Diagnostic {
public:
    Hint(ErrorCode code, std::string message);
    Hint(std::string_view code, std::string message);
};

class Fatal final : public Diagnostic {
public:
    Fatal(ErrorCode code, std::string message);
};

class Info final : public Diagnostic {
public:
    Info(ErrorCode code, std::string message);
};

}  // namespace myc::diagnostics
