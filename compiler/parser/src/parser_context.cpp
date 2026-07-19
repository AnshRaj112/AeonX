#include "myc/parser/parser_context.hpp"

#include "myc/diagnostics/error_code.hpp"
#include "myc/parser/recovery_manager.hpp"

namespace myc::parser {

ParserContext::~ParserContext() = default;

ParserContext::ParserContext(lexer::TokenStream tokens, source::SourceManager& source_manager,
                             diagnostics::DiagnosticEmitter& diagnostics,
                             ParserConfiguration configuration)
    : cursor_(TokenCursor(std::move(tokens))),
      configuration_(configuration),
      source_manager_(source_manager),
      diagnostics_(diagnostics),
      recovery_(std::make_unique<RecoveryManager>(*this)) {}

bool ParserContext::IsAtEnd() const noexcept { return cursor_.IsAtEnd(); }

bool ParserContext::IsDone() const noexcept { return cursor_.IsDone(); }

const lexer::Token& ParserContext::Peek(std::size_t offset) const { return cursor_.Peek(offset); }

const lexer::Token& ParserContext::LookAhead(std::size_t count) const {
    return cursor_.LookAhead(count);
}

const lexer::Token& ParserContext::Consume() { return cursor_.Consume(); }

const lexer::Token& ParserContext::Previous() const { return cursor_.Previous(); }

bool ParserContext::Match(lexer::TokenType type) { return cursor_.Match(type); }

bool ParserContext::MatchOneOf(std::initializer_list<lexer::TokenType> types) {
    return cursor_.MatchOneOf(types);
}

bool ParserContext::Expect(lexer::TokenType type, std::string_view message) {
    if (cursor_.Match(type)) {
        return true;
    }
    ReportError(Peek(), message);
    return false;
}

std::size_t ParserContext::Checkpoint() const noexcept { return cursor_.Checkpoint(); }

void ParserContext::Rewind(std::size_t checkpoint) noexcept { cursor_.Rewind(checkpoint); }

void ParserContext::ReportError(const lexer::Token& token, std::string_view message) {
    state_.had_error = true;
    ++state_.error_count;
    statistics_.RecordSyntaxError();

    (void)diagnostics_.Builder()
        .Error(diagnostics::codes::UnexpectedToken, std::string(message))
        .At(ToDiagnosticSpan(token))
        .Label(std::string(token.GetLexeme()))
        .Emit();

    if (state_.error_count >= configuration_.max_syntax_errors) {
        state_.in_panic_mode = true;
    }
}

void ParserContext::ReportErrorAt(source::SourceSpan span, std::string_view message) {
    state_.had_error = true;
    ++state_.error_count;
    statistics_.RecordSyntaxError();

    (void)diagnostics_.Builder()
        .Error(diagnostics::codes::InvalidSyntax, std::string(message))
        .At(ToDiagnosticSpan(span))
        .Emit();
}

void ParserContext::Recover() {
    if (!configuration_.enable_error_recovery) {
        return;
    }
    recovery_->Synchronize();
    state_.in_panic_mode = false;
    state_.recovered = true;
}

diagnostics::SourceSpan ParserContext::ToDiagnosticSpan(const source::SourceSpan& span) const {
    diagnostics::SourceSpan diag_span;
    if (!span.IsValid()) {
        return diag_span;
    }

    const auto file_id = span.GetStart().GetFileId().Value();
    diag_span.file_id = file_id;
    diag_span.start_offset = span.GetStart().GetOffset();
    diag_span.end_offset = span.GetEnd().GetOffset();
    diag_span.start.line = span.GetStart().GetLine();
    diag_span.start.column = span.GetStart().GetColumn();
    diag_span.end.line = span.GetEnd().GetLine();
    diag_span.end.column = span.GetEnd().GetColumn();

    if (const source::SourceFile* file = source_manager_.GetFile(file_id)) {
        diag_span.start.filename = std::string(file->GetFilename());
        diag_span.start.absolute_path = file->GetAbsolutePath().string();
        diag_span.end.filename = diag_span.start.filename;
        diag_span.end.absolute_path = diag_span.start.absolute_path;
    }
    if (const source::SourceBuffer* buffer = source_manager_.GetBuffer(file_id)) {
        diag_span.source_length = buffer->GetSize();
    }
    return diag_span;
}

diagnostics::SourceSpan ParserContext::ToDiagnosticSpan(const lexer::Token& token) const {
    return ToDiagnosticSpan(token.GetSpan());
}

}  // namespace myc::parser
