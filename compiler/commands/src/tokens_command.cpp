#include "myc/commands/tokens_command.hpp"

#include "myc/diagnostics/diagnostic_emitter.hpp"
#include "myc/diagnostics/diagnostic_reporter.hpp"
#include "myc/lexer/lexer.hpp"
#include "myc/lexer/token_printer.hpp"
#include "myc/logging/logger.hpp"

#include <iostream>
#include <optional>
#include <vector>

namespace myc::commands {

namespace {

[[nodiscard]] std::vector<std::string> CollectInputFiles(const CommandContext& ctx) {
    if (!ctx.options.source_files.empty()) {
        return ctx.options.source_files;
    }
    return ctx.options.positional_args;
}

void PrintSummary(std::ostream& output, std::string_view path, const myc::lexer::LexerResult& result) {
    output << path << ": " << result.tokens.Size() << " token(s)";
    if (result.had_errors) {
        output << ", " << result.statistics.GetErrorsEncountered() << " lexical error(s)";
    }
    if (result.statistics.GetElapsedTime().count() > 0) {
        output << ", " << result.statistics.GetElapsedTime().count() << " ns";
    }
    output << '\n';
}

}  // namespace

std::string_view TokensCommand::GetName() const noexcept { return "tokens"; }

std::string_view TokensCommand::GetDescription() const noexcept {
    return "Dump lexer token streams for debugging";
}

std::string_view TokensCommand::GetSynopsis() const noexcept {
    return "myc tokens <file.myc> [more.myc ...]";
}

exitcodes::ExitCode TokensCommand::Execute(const CommandContext& ctx) const {
    const std::vector<std::string> files = CollectInputFiles(ctx);
    if (files.empty()) {
        std::cerr << "error: no input file specified\n"
                  << "usage: " << GetSynopsis() << '\n';
        return exitcodes::ExitCode::InvalidArguments;
    }

    diagnostics::DiagnosticEmitter emitter(ctx.diagnostics);
    bool had_lexical_errors = false;

    for (const std::string& file : files) {
        const std::optional<myc::FileId> file_id = ctx.source_manager.LoadFile(file);
        if (!file_id) {
            return exitcodes::ExitCode::FileNotFound;
        }

        const source::SourceBuffer* buffer = ctx.source_manager.GetBuffer(*file_id);
        if (buffer == nullptr) {
            return exitcodes::ExitCode::FileNotFound;
        }

        myc::lexer::LexerOptions lexer_options;
        lexer_options.collect_statistics = ctx.options.verbose || ctx.options.time_report;
        myc::lexer::Lexer lexer(*buffer, ctx.source_manager, emitter, lexer_options);
        const myc::lexer::LexerResult result = lexer.Tokenize();

        if (result.had_errors) {
            had_lexical_errors = true;
        }

        if (!ctx.options.quiet) {
            PrintSummary(std::cout, file, result);
        }

        for (std::size_t index = 0; index < result.tokens.Size(); ++index) {
            const myc::lexer::Token& token = result.tokens.Peek(index);
            std::cout << "  " << myc::lexer::TokenPrinter::PrintWithLocation(token) << '\n';
        }

        if (!ctx.options.quiet && files.size() > 1 && &file != &files.back()) {
            std::cout << '\n';
        }

        logging::Logger::Instance().Info("tokens: lexed " + file + " (" +
                                         std::to_string(result.tokens.Size()) + " tokens)");
    }

    if (had_lexical_errors && ctx.diagnostics.HasErrors()) {
        diagnostics::DiagnosticReporter reporter;
        diagnostics::DiagnosticReporter::Options reporter_options;
        reporter_options.source_manager = &ctx.source_manager;
        reporter.SetOptions(reporter_options);
        reporter.Report(ctx.diagnostics, std::cerr);
    }

    return had_lexical_errors ? exitcodes::ExitCode::GeneralError
                              : exitcodes::ExitCode::Success;
}

}  // namespace myc::commands
