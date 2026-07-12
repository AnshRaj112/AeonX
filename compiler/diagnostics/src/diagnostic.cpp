#include "myc/diagnostics/diagnostic.hpp"

#include <sstream>

namespace myc::diagnostics {

Diagnostic::Diagnostic(Severity severity, DiagnosticCode code, std::string message)
    : severity_(severity), code_(std::move(code)), message_(std::move(message)) {}

Severity Diagnostic::GetSeverity() const noexcept { return severity_; }

const DiagnosticCode& Diagnostic::GetCode() const noexcept { return code_; }

const std::string& Diagnostic::GetMessage() const noexcept { return message_; }

void Diagnostic::SetLocation(FileId file, LineNumber line, ColumnNumber column) {
    file_id_ = file;
    line_ = line;
    column_ = column;
}

std::optional<FileId> Diagnostic::GetFileId() const noexcept { return file_id_; }

std::optional<LineNumber> Diagnostic::GetLine() const noexcept { return line_; }

std::optional<ColumnNumber> Diagnostic::GetColumn() const noexcept { return column_; }

void Diagnostic::AddFixIt(FixItHint hint) { fix_its_.push_back(std::move(hint)); }

const std::vector<FixItHint>& Diagnostic::GetFixIts() const noexcept { return fix_its_; }

std::string Diagnostic::Format() const {
    std::ostringstream stream;
    stream << ToString(severity_);
    if (!code_.empty()) {
        stream << " [" << code_ << "]";
    }
    if (file_id_.has_value() && line_.has_value() && column_.has_value()) {
        stream << " at file:" << *file_id_ << ':' << *line_ << ':' << *column_;
    }
    stream << ": " << message_;
    return stream.str();
}

Error::Error(DiagnosticCode code, std::string message)
    : Diagnostic(Severity::Error, std::move(code), std::move(message)) {}

Warning::Warning(DiagnosticCode code, std::string message)
    : Diagnostic(Severity::Warning, std::move(code), std::move(message)) {}

Note::Note(DiagnosticCode code, std::string message)
    : Diagnostic(Severity::Note, std::move(code), std::move(message)) {}

Hint::Hint(DiagnosticCode code, std::string message)
    : Diagnostic(Severity::Hint, std::move(code), std::move(message)) {}

}  // namespace myc::diagnostics
