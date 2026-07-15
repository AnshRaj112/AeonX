#include "myc/lexer/scanners/scanner_utils.hpp"

#include "myc/diagnostics/diagnostic_emitter.hpp"
#include "myc/lexer/token_factory.hpp"
#include "myc/lexer/token_flags.hpp"

namespace myc::lexer::scanner_utils {

namespace {

constexpr std::size_t kMaxOperatorLength = 4;

[[nodiscard]] diagnostics::SourceSpan MakeDiagnosticSpan(LexerContext& context,
                                                         std::size_t start,
                                                         std::size_t end) {
    diagnostics::SourceSpan diag_span;
    const myc::FileId file_id = context.GetFileId().Value();
    diag_span.file_id = file_id;

    const source::SourceLocation src_start =
        context.GetSourceManager().GetLocation(file_id, start);
    const source::SourceLocation src_end = context.GetSourceManager().GetLocation(file_id, end);

    if (const source::SourceFile* file = context.GetSourceManager().GetFile(file_id)) {
        diag_span.start.filename = std::string(file->GetFilename());
        diag_span.start.absolute_path = file->GetAbsolutePath().string();
        diag_span.end.filename = diag_span.start.filename;
        diag_span.end.absolute_path = diag_span.start.absolute_path;
    }

    diag_span.start.line = src_start.GetLine();
    diag_span.start.column = src_start.GetColumn();
    diag_span.end.line = src_end.GetLine();
    diag_span.end.column = src_end.GetColumn();
    diag_span.start_offset = start;
    diag_span.end_offset = end;

    if (const source::SourceBuffer* buffer = context.GetSourceManager().GetBuffer(file_id)) {
        diag_span.source_length = buffer->GetSize();
    }

    return diag_span;
}

}  // namespace

bool IsIdentifierStart(char32_t ch) noexcept {
    if (IsAsciiAlpha(ch) || ch == U'_') {
        return true;
    }
    // Unicode placeholder: non-ASCII code points that are not digits may start identifiers.
    if (ch > 0x7F && !IsAsciiDigit(ch) && ch != 0) {
        return true;
    }
    return false;
}

bool IsIdentifierContinue(char32_t ch) noexcept {
    return IsIdentifierStart(ch) || IsAsciiDigit(ch);
}

void AdvanceAndRecord(LexerContext& context) {
    CharacterStream& stream = context.GetStream();
    (void)stream.Advance();
    context.GetStatistics().RecordCharacterRead();
}

void RecordAdvance(LexerContext& context, std::size_t code_points) {
    for (std::size_t i = 0; i < code_points; ++i) {
        AdvanceAndRecord(context);
    }
}

source::SourceSpan MakeSpan(LexerContext& context, std::size_t start, std::size_t end) {
    return context.GetSourceManager().MakeSpan(context.GetFileId().Value(), start, end);
}

std::size_t MeasureIdentifierEnd(const CharacterStream& stream) {
    CharacterStream& mutable_stream = const_cast<CharacterStream&>(stream);
    const std::size_t mark = mutable_stream.Mark();
    while (!mutable_stream.Eof() &&
           IsIdentifierContinue(mutable_stream.Peek().GetUnicodeValue())) {
        (void)mutable_stream.Advance();
    }
    const std::size_t end = mutable_stream.Position();
    mutable_stream.Reset(mark);
    return end;
}

std::size_t ScanIdentifierEnd(LexerContext& context) {
    const std::size_t end = MeasureIdentifierEnd(context.GetStream());
    ConsumeTo(context, end);
    return end;
}

void ConsumeTo(LexerContext& context, std::size_t end) {
    CharacterStream& stream = context.GetStream();
    while (stream.Position() < end && !stream.Eof()) {
        AdvanceAndRecord(context);
    }
}

std::string_view SliceLexeme(LexerContext& context, std::size_t start, std::size_t end) {
    return context.GetStream().Slice(start, end);
}

std::optional<OperatorMatch> MatchLongestOperator(CharacterStream& stream) {
    const std::size_t start = stream.Position();
    const CharacterReader& reader = stream.GetReader();
    const std::size_t buffer_size = reader.GetSize();

    const Operator* best = nullptr;
    std::size_t best_length = 0;

    for (std::size_t length = 1; length <= kMaxOperatorLength && start + length <= buffer_size;
         ++length) {
        const std::string_view candidate = stream.Slice(start, start + length);
        if (const Operator* op = OperatorTable::Lookup(candidate)) {
            best = op;
            best_length = length;
        }
    }

    if (best == nullptr) {
        return std::nullopt;
    }
    return OperatorMatch{best, best_length};
}

void EmitLexicalError(LexerContext& context, std::size_t start, std::size_t end,
                      std::string_view message) {
    context.MarkError();
    context.GetStatistics().RecordError();

    const diagnostics::SourceSpan diag_span = MakeDiagnosticSpan(context, start, end);
    (void)context.GetDiagnostics()
        .Builder()
        .Error(diagnostics::ErrorCode{"MYC0001"}, std::string(message))
        .At(diag_span)
        .Emit();
}

void PushErrorToken(LexerContext& context, std::size_t start, std::size_t end,
                    std::string_view message) {
    EmitLexicalError(context, start, end, message);
    const auto location =
        context.GetSourceManager().GetLocation(context.GetFileId().Value(), start);
    const auto span = MakeSpan(context, start, end);
    const std::string_view lexeme = SliceLexeme(context, start, end);
    context.PushToken(TokenFactory::MakeError(lexeme, location, span,
                                            TokenFlags::Invalid | TokenFlags::Recoverable));
}

}  // namespace myc::lexer::scanner_utils
