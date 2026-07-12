#include "myc_test.hpp"

#include "myc/diagnostics/ansi_color.hpp"
#include "myc/diagnostics/diagnostic.hpp"
#include "myc/diagnostics/diagnostic_builder.hpp"
#include "myc/diagnostics/diagnostic_emitter.hpp"
#include "myc/diagnostics/diagnostic_engine.hpp"
#include "myc/diagnostics/diagnostic_formatter.hpp"
#include "myc/diagnostics/diagnostic_json.hpp"
#include "myc/diagnostics/diagnostic_reporter.hpp"
#include "myc/diagnostics/error_code.hpp"
#include "myc/diagnostics/severity.hpp"
#include "myc/diagnostics/source_location.hpp"
#include "myc/diagnostics/source_span.hpp"
#include "myc/source/source_manager.hpp"

#include <sstream>
#include <string>

namespace {

std::string CaptureReport(const myc::diagnostics::DiagnosticEngine& engine,
                          const myc::diagnostics::DiagnosticReporter::Options& options) {
    std::ostringstream stream;
    myc::diagnostics::DiagnosticReporter reporter(options);
    reporter.Report(engine, stream);
    return stream.str();
}

}  // namespace

MYC_TEST(diagnostics_starts_empty) {
    myc::diagnostics::DiagnosticEngine engine;
    MYC_ASSERT(!engine.HasErrors());
    MYC_ASSERT_EQ(engine.GetErrorCount(), 0u);
    MYC_ASSERT_EQ(engine.GetExitCode(), 0);
}

MYC_TEST(diagnostics_records_errors) {
    myc::diagnostics::DiagnosticEngine engine;
    engine.EmitError("E0001", "test error");
    MYC_ASSERT(engine.HasErrors());
    MYC_ASSERT_EQ(engine.GetErrorCount(), 1u);
    MYC_ASSERT_EQ(engine.GetExitCode(), 1);
}

MYC_TEST(diagnostics_records_warnings_without_error_exit) {
    myc::diagnostics::DiagnosticEngine engine;
    engine.EmitWarning("W0001", "test warning");
    MYC_ASSERT(!engine.HasErrors());
    MYC_ASSERT_EQ(engine.GetWarningCount(), 1u);
    MYC_ASSERT_EQ(engine.GetExitCode(), 0);
}

MYC_TEST(diagnostic_supports_location) {
    myc::diagnostics::Error error("E0002", "located error");
    error.SetLocation(1, 10, 5);
    MYC_ASSERT(error.GetFileId().has_value());
    MYC_ASSERT_EQ(*error.GetLine(), 10u);
    MYC_ASSERT_EQ(*error.GetColumn(), 5u);
}

MYC_TEST(severity_to_string_covers_all_levels) {
    MYC_ASSERT(myc::diagnostics::ToString(myc::diagnostics::Severity::Fatal) == "fatal");
    MYC_ASSERT(myc::diagnostics::ToString(myc::diagnostics::Severity::Error) == "error");
    MYC_ASSERT(myc::diagnostics::ToString(myc::diagnostics::Severity::Warning) == "warning");
    MYC_ASSERT(myc::diagnostics::ToString(myc::diagnostics::Severity::Note) == "note");
    MYC_ASSERT(myc::diagnostics::ToString(myc::diagnostics::Severity::Hint) == "hint");
    MYC_ASSERT(myc::diagnostics::ToString(myc::diagnostics::Severity::Info) == "info");
}

MYC_TEST(error_code_from_number_formats_myc_prefix) {
    const myc::diagnostics::ErrorCode code = myc::diagnostics::ErrorCode::FromNumber(42);
    MYC_ASSERT(code.AsString() == "MYC0042");
}

MYC_TEST(error_code_registry_contains_examples) {
    MYC_ASSERT(myc::diagnostics::codes::UnexpectedToken.AsString() == "MYC0001");
    MYC_ASSERT(myc::diagnostics::codes::TypeMismatch.AsString() == "MYC1000");
}

MYC_TEST(source_span_tracks_multiline_ranges) {
    myc::diagnostics::SourceSpan span;
    span.start.line = 1;
    span.start.column = 1;
    span.end.line = 3;
    span.end.column = 4;
    span.start_offset = 0;
    span.end_offset = 20;
    span.source_length = 100;
    MYC_ASSERT(span.IsMultiLine());
    MYC_ASSERT(span.IsValid() == false);
    span.start.filename = "hello.myc";
    MYC_ASSERT(span.IsValid());
}

MYC_TEST(builder_emits_structured_diagnostic) {
    myc::diagnostics::DiagnosticEngine engine;
    myc::diagnostics::SourceSpan span;
    span.start.filename = "hello.myc";
    span.start.line = 12;
    span.start.column = 5;
    span.end = span.start;

    const bool emitted = myc::diagnostics::DiagnosticBuilder(engine, myc::diagnostics::Severity::Error)
                             .Error(myc::diagnostics::codes::UnexpectedToken, "unexpected token")
                             .At(span)
                             .Label("expected expression")
                             .Note("expressions cannot be empty")
                             .Hint("insert a valid expression before ')'")
                             .Emit();
    MYC_ASSERT(emitted);
    MYC_ASSERT_EQ(engine.GetDiagnostics().size(), 1u);
    MYC_ASSERT_EQ(engine.GetDiagnostics().front()->GetNotes().size(), 1u);
    MYC_ASSERT_EQ(engine.GetDiagnostics().front()->GetHints().size(), 1u);
}

MYC_TEST(formatter_renders_primary_message) {
    myc::diagnostics::Error error(myc::diagnostics::codes::UnexpectedToken, "unexpected token");
    myc::diagnostics::DiagnosticFormatter formatter;
    const std::string rendered = formatter.Format(error);
    MYC_ASSERT(rendered.find("error[MYC0001]") != std::string::npos);
    MYC_ASSERT(rendered.find("unexpected token") != std::string::npos);
}

MYC_TEST(formatter_renders_source_snippet) {
    myc::source::SourceManager source_manager;
    const myc::FileId file_id = source_manager.AddMemoryBuffer("hello.myc", "print(\n");

    myc::diagnostics::Error error(myc::diagnostics::codes::UnexpectedToken, "unexpected token");
    myc::diagnostics::SourceSpan span;
    span.file_id = file_id;
    span.start.filename = "hello.myc";
    span.start.line = 1;
    span.start.column = 6;
    span.end = span.start;
    span.start_offset = 5;
    span.end_offset = 6;
    error.SetSpan(span);
    error.SetLabel("expected expression");

    myc::diagnostics::DiagnosticFormatter formatter({.source_manager = &source_manager});
    const std::string rendered = formatter.Format(error);
    MYC_ASSERT(rendered.find("hello.myc:1:6") != std::string::npos);
    MYC_ASSERT(rendered.find("print(") != std::string::npos);
    MYC_ASSERT(rendered.find("^") != std::string::npos);
}

MYC_TEST(json_writer_emits_machine_readable_output) {
    myc::diagnostics::DiagnosticEngine engine;
    myc::diagnostics::SourceSpan span;
    span.start.filename = "hello.myc";
    span.start.absolute_path = "/tmp/hello.myc";
    span.start.line = 12;
    span.start.column = 5;
    span.end = span.start;

    const bool emitted = myc::diagnostics::DiagnosticBuilder(engine, myc::diagnostics::Severity::Error)
                             .Error(myc::diagnostics::codes::UnexpectedToken, "unexpected token")
                             .At(span)
                             .Note("expressions cannot be empty")
                             .Emit();
    MYC_ASSERT(emitted);

    const std::string json = myc::diagnostics::DiagnosticJsonWriter::Write(engine.GetDiagnostics());
    MYC_ASSERT(json.find(R"("severity":"error")") != std::string::npos);
    MYC_ASSERT(json.find(R"("code":"MYC0001")") != std::string::npos);
    MYC_ASSERT(json.find(R"("message":"unexpected token")") != std::string::npos);
    MYC_ASSERT(json.find(R"("filename":"hello.myc")") != std::string::npos);
    MYC_ASSERT(json.find(R"("notes")") != std::string::npos);
}

MYC_TEST(engine_enforces_warning_limit) {
    myc::diagnostics::DiagnosticEngine engine;
    myc::diagnostics::DiagnosticsPolicy policy;
    policy.warning_limit = 1;
    engine.SetPolicy(policy);

    MYC_ASSERT(engine.EmitWarning(myc::diagnostics::ErrorCode("W0001"), "first"));
    MYC_ASSERT(!engine.EmitWarning(myc::diagnostics::ErrorCode("W0002"), "second"));
    MYC_ASSERT_EQ(engine.GetWarningCount(), 1u);
}

MYC_TEST(engine_treats_warnings_as_errors) {
    myc::diagnostics::DiagnosticEngine engine;
    myc::diagnostics::DiagnosticsPolicy policy;
    policy.warnings_as_errors = true;
    engine.SetPolicy(policy);

    engine.EmitWarning("W0001", "promoted");
    MYC_ASSERT(engine.HasErrors());
    MYC_ASSERT_EQ(engine.GetErrorCount(), 1u);
}

MYC_TEST(engine_suppresses_warnings) {
    myc::diagnostics::DiagnosticEngine engine;
    myc::diagnostics::DiagnosticsPolicy policy;
    policy.suppress_warnings = true;
    engine.SetPolicy(policy);

    MYC_ASSERT(!engine.EmitWarning(myc::diagnostics::ErrorCode("W0001"), "hidden"));
    MYC_ASSERT_EQ(engine.GetWarningCount(), 0u);
}

MYC_TEST(engine_supports_error_recovery) {
    myc::diagnostics::DiagnosticEngine engine;
    myc::diagnostics::DiagnosticsPolicy policy;
    policy.error_recovery = true;
    engine.SetPolicy(policy);

    engine.EmitError("E0001", "recoverable");
    MYC_ASSERT(engine.ShouldContinue());
}

MYC_TEST(engine_stops_on_fatal_without_recovery) {
    myc::diagnostics::DiagnosticEngine engine;
    myc::diagnostics::DiagnosticsPolicy policy;
    policy.error_recovery = false;
    engine.SetPolicy(policy);

    (void)engine.EmitFatal(myc::diagnostics::codes::DriverInternalError, "fatal");
    MYC_ASSERT(!engine.ShouldContinue());
}

MYC_TEST(reporter_outputs_plain_text) {
    myc::diagnostics::DiagnosticEngine engine;
    (void)engine.EmitError(myc::diagnostics::codes::UnexpectedToken, "unexpected token");

    myc::diagnostics::DiagnosticReporter::Options options;
    options.format = myc::diagnostics::DiagnosticOutputFormat::PlainText;
    const std::string output = CaptureReport(engine, options);
    MYC_ASSERT(output.find("unexpected token") != std::string::npos);
}

MYC_TEST(reporter_outputs_json) {
    myc::diagnostics::DiagnosticEngine engine;
    (void)engine.EmitError(myc::diagnostics::codes::UnexpectedToken, "unexpected token");

    myc::diagnostics::DiagnosticReporter::Options options;
    options.format = myc::diagnostics::DiagnosticOutputFormat::Json;
    const std::string output = CaptureReport(engine, options);
    MYC_ASSERT(output.find(R"("diagnostics")") != std::string::npos);
}

MYC_TEST(emitter_provides_builder_access) {
    myc::diagnostics::DiagnosticEngine engine;
    myc::diagnostics::DiagnosticEmitter emitter(engine);
    MYC_ASSERT(emitter.EmitError(myc::diagnostics::codes::InvalidSyntax, "bad syntax"));
    MYC_ASSERT_EQ(engine.GetErrorCount(), 1u);
}

MYC_TEST(ansi_color_can_be_disabled) {
    const std::string plain = myc::diagnostics::ansi::Colorize("text", myc::diagnostics::ansi::Red, false);
    MYC_ASSERT(plain == "text");
}

MYC_TEST(json_escape_handles_special_characters) {
    const std::string escaped = myc::diagnostics::DiagnosticJsonWriter::Escape("line\n\"quote\"");
    MYC_ASSERT(escaped.find("\\n") != std::string::npos);
    MYC_ASSERT(escaped.find("\\\"quote\\\"") != std::string::npos);
}
