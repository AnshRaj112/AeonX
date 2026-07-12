#include "myc/diagnostics/diagnostic.hpp"

#include "myc/diagnostics/diagnostic_formatter.hpp"

#include <sstream>
#include <utility>

namespace myc::diagnostics {

Diagnostic::Diagnostic(const Severity severity, ErrorCode code, std::string message)
    : severity_(severity), code_(std::move(code)), message_(std::move(message)) {}

Severity Diagnostic::GetSeverity() const noexcept { return severity_; }

const ErrorCode& Diagnostic::GetCode() const noexcept { return code_; }

const std::string& Diagnostic::GetMessage() const noexcept { return message_; }

const std::optional<SourceSpan>& Diagnostic::GetSpan() const noexcept { return span_; }

const std::string& Diagnostic::GetLabel() const noexcept { return label_; }

const std::vector<Diagnostic>& Diagnostic::GetNotes() const noexcept { return notes_; }

const std::vector<Diagnostic>& Diagnostic::GetHints() const noexcept { return hints_; }

const std::vector<FixItHint>& Diagnostic::GetFixIts() const noexcept { return fix_its_; }

void Diagnostic::SetSpan(SourceSpan span) { span_ = std::move(span); }

void Diagnostic::SetLabel(std::string label) { label_ = std::move(label); }

void Diagnostic::AddNote(Diagnostic note) { notes_.push_back(std::move(note)); }

void Diagnostic::AddHint(Diagnostic hint) { hints_.push_back(std::move(hint)); }

void Diagnostic::AddFixIt(FixItHint hint) { fix_its_.push_back(std::move(hint)); }

void Diagnostic::SetLocation(const FileId file, const LineNumber line,
                             const ColumnNumber column) {
    SourceSpan span;
    span.file_id = file;
    span.start.line = line;
    span.start.column = column;
    span.end = span.start;
    span_ = span;
}

std::optional<FileId> Diagnostic::GetFileId() const noexcept {
    if (!span_.has_value()) {
        return std::nullopt;
    }
    return span_->file_id;
}

std::optional<LineNumber> Diagnostic::GetLine() const noexcept {
    if (!span_.has_value()) {
        return std::nullopt;
    }
    return span_->start.line;
}

std::optional<ColumnNumber> Diagnostic::GetColumn() const noexcept {
    if (!span_.has_value()) {
        return std::nullopt;
    }
    return span_->start.column;
}

std::string Diagnostic::Format() const {
  DiagnosticFormatter formatter;
  return formatter.Format(*this);
}

Fatal::Fatal(ErrorCode code, std::string message)
    : Diagnostic(Severity::Fatal, std::move(code), std::move(message)) {}

Error::Error(ErrorCode code, std::string message)
    : Diagnostic(Severity::Error, std::move(code), std::move(message)) {}

Error::Error(const std::string_view code, std::string message)
    : Diagnostic(Severity::Error, ErrorCode(code), std::move(message)) {}

Warning::Warning(ErrorCode code, std::string message)
    : Diagnostic(Severity::Warning, std::move(code), std::move(message)) {}

Warning::Warning(const std::string_view code, std::string message)
    : Diagnostic(Severity::Warning, ErrorCode(code), std::move(message)) {}

Note::Note(ErrorCode code, std::string message)
    : Diagnostic(Severity::Note, std::move(code), std::move(message)) {}

Note::Note(const std::string_view code, std::string message)
    : Diagnostic(Severity::Note, ErrorCode(code), std::move(message)) {}

Hint::Hint(ErrorCode code, std::string message)
    : Diagnostic(Severity::Hint, std::move(code), std::move(message)) {}

Hint::Hint(const std::string_view code, std::string message)
    : Diagnostic(Severity::Hint, ErrorCode(code), std::move(message)) {}

Info::Info(ErrorCode code, std::string message)
    : Diagnostic(Severity::Info, std::move(code), std::move(message)) {}

}  // namespace myc::diagnostics
