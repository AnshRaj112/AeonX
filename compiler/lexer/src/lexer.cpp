#include "myc/lexer/lexer.hpp"

#include "myc/diagnostics/diagnostic_emitter.hpp"
#include "myc/lexer/token_flags.hpp"

namespace myc::lexer {

namespace {

void FlushPendingTokens(LexerContext& context, std::vector<Token>& tokens) {
    for (Token& token : context.GetPendingTokens()) {
        tokens.push_back(std::move(token));
    }
    context.ClearPendingTokens();
}

}  // namespace

Lexer::Lexer(const source::SourceBuffer& buffer, source::SourceManager& source_manager,
             diagnostics::DiagnosticEmitter& diagnostics, LexerOptions options)
    : reader_(buffer, source_manager),
      stream_(reader_),
      source_manager_(source_manager),
      diagnostics_(diagnostics),
      configuration_(LexerConfiguration(options)) {}

LexerResult Lexer::Tokenize() {
    if (configuration_.ShouldCollectStatistics()) {
        statistics_.BeginTiming();
    }

    LexerContext context(stream_, source_manager_, diagnostics_, configuration_, statistics_);
    std::vector<Token> tokens;
    tokens.reserve(64);

    while (!stream_.Eof()) {
        const std::size_t line_before = stream_.Peek().GetLine();
        const ScanResult result = scanners_.Dispatch(context);

        if (result == ScanResult::TokenProduced) {
            FlushPendingTokens(context, tokens);
            continue;
        }

        if (result == ScanResult::Consumed) {
            continue;
        }

        if (result == ScanResult::Error) {
            context.MarkError();
            FlushPendingTokens(context, tokens);
            if (!configuration_.ShouldContinueAfterErrors()) {
                break;
            }
            RecoverFromUnknownCharacter(context);
            FlushPendingTokens(context, tokens);
            continue;
        }

        RecoverFromUnknownCharacter(context);
        FlushPendingTokens(context, tokens);

        if (configuration_.ShouldCollectStatistics()) {
            const std::size_t line_after = stream_.Peek().GetLine();
            if (line_after > line_before) {
                statistics_.RecordLineProcessed();
            }
        }
    }

    if (configuration_.ShouldCollectStatistics()) {
        statistics_.RecordLineProcessed();
        statistics_.EndTiming();
        statistics_.SetMemoryUsageBytes(
            static_cast<std::uint64_t>(tokens.capacity() * sizeof(Token)));
    }

    tokens.push_back(MakeEofToken());

    LexerResult result_obj;
    result_obj.tokens = TokenStream(std::move(tokens));
    result_obj.statistics = statistics_;
    result_obj.had_errors = context.HadErrors();
    return result_obj;
}

void Lexer::RecoverFromUnknownCharacter(LexerContext& context) {
    if (stream_.Eof()) {
        return;
    }

    const Character unknown = stream_.Advance();
    context.MarkError();
    statistics_.RecordCharacterRead();
    statistics_.RecordError();

    const std::size_t start = unknown.GetByteOffset();
    const std::size_t end = stream_.Position();
    const auto location = unknown.GetLocation();
    const auto span = source_manager_.MakeSpan(location.GetFileId().Value(), start, end);

    (void)context.GetDiagnostics().EmitError(
        diagnostics::ErrorCode{"MYC0001"},
        std::string("unexpected character in lexical analysis (infrastructure pass)"));

    if (configuration_.ShouldContinueAfterErrors()) {
        context.PushToken(TokenFactory::MakeError(reader_.Slice(start, end), location, span,
                                                  TokenFlags::Invalid | TokenFlags::Recoverable));
    }
}

Token Lexer::MakeEofToken() const {
    const std::size_t offset = stream_.Position();
    const auto location = reader_.MakeLocation(offset);
    const auto span =
        source_manager_.MakeSpan(location.GetFileId().Value(), offset, offset);
    return TokenFactory::MakeEOF(location, span);
}

}  // namespace myc::lexer
