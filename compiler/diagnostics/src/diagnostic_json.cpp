#include "myc/diagnostics/diagnostic_json.hpp"

#include <format>
#include <sstream>

namespace myc::diagnostics {

namespace {

void WriteSpan(std::ostringstream& stream, const SourceSpan& span) {
    stream << R"("span":{)";
    stream << R"("filename":")" << DiagnosticJsonWriter::Escape(span.start.filename) << '"';
    stream << R"(,"absolute_path":")" << DiagnosticJsonWriter::Escape(span.start.absolute_path)
           << '"';
    stream << std::format(R"(,"start_line":{},"start_column":{})", span.start.line,
                          span.start.column);
    stream << std::format(R"(,"end_line":{},"end_column":{})", span.end.line, span.end.column);
    stream << std::format(R"(,"start_offset":{},"end_offset":{},"source_length":{})",
                          span.start_offset, span.end_offset, span.source_length);
    if (span.file_id.has_value()) {
        stream << std::format(R"(,"file_id":{})", *span.file_id);
    }
    stream << '}';
}

void WriteRelated(std::ostringstream& stream, const char* field,
                  const std::vector<Diagnostic>& related) {
    stream << '"' << field << R"(":[)";
    bool first = true;
    for (const Diagnostic& item : related) {
        if (!first) {
            stream << ',';
        }
        first = false;
        stream << '{';
        stream << R"("severity":")" << DiagnosticJsonWriter::Escape(ToString(item.GetSeverity()))
               << '"';
        if (!item.GetCode().GetCode().empty()) {
            stream << R"(,"code":")" << DiagnosticJsonWriter::Escape(item.GetCode().AsString())
                   << '"';
        }
        stream << R"(,"message":")" << DiagnosticJsonWriter::Escape(item.GetMessage()) << '"';
        stream << '}';
    }
    stream << ']';
}

void WriteDiagnostic(std::ostringstream& stream, const Diagnostic& diagnostic) {
    stream << '{';
    stream << R"("severity":")" << DiagnosticJsonWriter::Escape(ToString(diagnostic.GetSeverity()))
           << '"';
    if (!diagnostic.GetCode().GetCode().empty()) {
        stream << R"(,"code":")" << DiagnosticJsonWriter::Escape(diagnostic.GetCode().AsString())
               << '"';
    }
    stream << R"(,"message":")" << DiagnosticJsonWriter::Escape(diagnostic.GetMessage()) << '"';
    if (!diagnostic.GetLabel().empty()) {
        stream << R"(,"label":")" << DiagnosticJsonWriter::Escape(diagnostic.GetLabel()) << '"';
    }
    if (const auto& span = diagnostic.GetSpan(); span.has_value()) {
        stream << ',';
        WriteSpan(stream, *span);
    }
    if (!diagnostic.GetNotes().empty()) {
        stream << ',';
        WriteRelated(stream, "notes", diagnostic.GetNotes());
    }
    if (!diagnostic.GetHints().empty()) {
        stream << ',';
        WriteRelated(stream, "hints", diagnostic.GetHints());
    }
    stream << '}';
}

}  // namespace

std::string DiagnosticJsonWriter::Write(
    const std::vector<std::unique_ptr<Diagnostic>>& diagnostics) {
    std::ostringstream stream;
    stream << R"({"diagnostics":[)";
    bool first = true;
    for (const auto& diagnostic : diagnostics) {
        if (!first) {
            stream << ',';
        }
        first = false;
        WriteDiagnostic(stream, *diagnostic);
    }
    stream << "]}";
    return stream.str();
}

std::string DiagnosticJsonWriter::Escape(const std::string_view text) {
    std::ostringstream stream;
    for (const char ch : text) {
        switch (ch) {
            case '"':
                stream << "\\\"";
                break;
            case '\\':
                stream << "\\\\";
                break;
            case '\b':
                stream << "\\b";
                break;
            case '\f':
                stream << "\\f";
                break;
            case '\n':
                stream << "\\n";
                break;
            case '\r':
                stream << "\\r";
                break;
            case '\t':
                stream << "\\t";
                break;
            default:
                if (static_cast<unsigned char>(ch) < 0x20) {
                    stream << std::format("\\u{:04x}", static_cast<unsigned char>(ch));
                } else {
                    stream << ch;
                }
                break;
        }
    }
    return stream.str();
}

}  // namespace myc::diagnostics
