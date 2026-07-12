#include "myc_test.hpp"

#include "myc/diagnostics/diagnostic_engine.hpp"

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
