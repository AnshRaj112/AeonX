#include "myc_test.hpp"

#include "myc/diagnostics/diagnostic_emitter.hpp"
#include "myc/diagnostics/diagnostic_engine.hpp"
#include "myc/lexer/lexer.hpp"
#include "myc/lexer/lexer_options.hpp"
#include "myc/lexer/lexer_state.hpp"
#include "myc/lexer/scanners/scanner_registry.hpp"
#include "myc/source/source_manager.hpp"

MYC_TEST(lexer_initialization) {
    myc::source::SourceManager source_manager;
    myc::diagnostics::DiagnosticEngine engine;
    myc::diagnostics::DiagnosticEmitter emitter(engine);

    const myc::FileId id = source_manager.AddMemoryBuffer("init.myc", "let x = 1;\n");
    const myc::source::SourceBuffer* buffer = source_manager.GetBuffer(id);
    MYC_ASSERT(buffer != nullptr);

    myc::lexer::Lexer lexer(*buffer, source_manager, emitter);
    MYC_ASSERT_EQ(lexer.GetCharacterStream().Position(), 0u);
    MYC_ASSERT(!lexer.GetCharacterStream().Eof());
}

MYC_TEST(lexer_tokenize_infrastructure_pass) {
    myc::source::SourceManager source_manager;
    myc::diagnostics::DiagnosticEngine engine;
    myc::diagnostics::DiagnosticEmitter emitter(engine);

    const myc::FileId id = source_manager.AddMemoryBuffer("pass.myc", "a");
    const myc::source::SourceBuffer* buffer = source_manager.GetBuffer(id);
    MYC_ASSERT(buffer != nullptr);

    myc::lexer::LexerOptions options;
    options.collect_statistics = true;
    myc::lexer::Lexer lexer(*buffer, source_manager, emitter, options);

    const myc::lexer::LexerResult result = lexer.Tokenize();
    MYC_ASSERT(!result.tokens.Empty());
    MYC_ASSERT(result.tokens.Peek().IsEOF() == false || result.tokens.Size() > 1);

    const auto& stats = result.statistics;
    MYC_ASSERT(stats.GetScannerCalls() > 0u);
    MYC_ASSERT(stats.GetElapsedTime().count() >= 0);
}

MYC_TEST(lexer_statistics_collection) {
    myc::source::SourceManager source_manager;
    myc::diagnostics::DiagnosticEngine engine;
    myc::diagnostics::DiagnosticEmitter emitter(engine);

    const myc::FileId id = source_manager.AddMemoryBuffer("stats.myc", "ab\n");
    const myc::source::SourceBuffer* buffer = source_manager.GetBuffer(id);
    MYC_ASSERT(buffer != nullptr);

    myc::lexer::LexerOptions options;
    options.collect_statistics = true;
    myc::lexer::Lexer lexer(*buffer, source_manager, emitter, options);

    const myc::lexer::LexerResult result = lexer.Tokenize();
    MYC_ASSERT(result.statistics.GetCharactersRead() >= 3u);
    MYC_ASSERT(result.statistics.GetErrorsEncountered() >= 3u);
    MYC_ASSERT(result.had_errors);
    MYC_ASSERT(result.tokens.Size() >= 4u);
}

MYC_TEST(lexer_error_recovery_continues) {
    myc::source::SourceManager source_manager;
    myc::diagnostics::DiagnosticEngine engine;
    myc::diagnostics::DiagnosticEmitter emitter(engine);

    const myc::FileId id = source_manager.AddMemoryBuffer("recover.myc", "!!");
    const myc::source::SourceBuffer* buffer = source_manager.GetBuffer(id);
    MYC_ASSERT(buffer != nullptr);

    myc::lexer::LexerOptions options;
    options.continue_after_errors = true;
    myc::lexer::Lexer lexer(*buffer, source_manager, emitter, options);

    const myc::lexer::LexerResult result = lexer.Tokenize();
    MYC_ASSERT(result.had_errors);
    MYC_ASSERT(result.statistics.GetErrorsEncountered() == 2u);
    MYC_ASSERT(result.tokens.IsAtEnd() == false || result.tokens.Size() > 0u);
}

MYC_TEST(lexer_state_string_representation) {
    MYC_ASSERT_EQ(std::string(myc::lexer::ToString(myc::lexer::LexerState::Normal)), "Normal");
    MYC_ASSERT_EQ(std::string(myc::lexer::ToString(myc::lexer::LexerState::InsideString)),
                  "InsideString");
}

MYC_TEST(scanner_registry_dispatch_not_handled) {
    myc::source::SourceManager source_manager;
    myc::diagnostics::DiagnosticEngine engine;
    myc::diagnostics::DiagnosticEmitter emitter(engine);

    const myc::FileId id = source_manager.AddMemoryBuffer("scan.myc", "z");
    const myc::source::SourceBuffer* buffer = source_manager.GetBuffer(id);
    MYC_ASSERT(buffer != nullptr);

    myc::lexer::CharacterReader reader(*buffer, source_manager);
    myc::lexer::CharacterStream stream(reader);
    myc::lexer::LexerConfiguration config;
    myc::lexer::LexerStatistics stats;
    myc::lexer::LexerContext context(stream, source_manager, emitter, config, stats);

    myc::lexer::ScannerRegistry registry;
    const myc::lexer::ScanResult result = registry.Dispatch(context);
    MYC_ASSERT(result == myc::lexer::ScanResult::NotHandled);
    MYC_ASSERT(stats.GetScannerCalls() > 0u);
}
