#include "myc/lexer/scanners/scanner_utils.hpp"

#include "myc/diagnostics/diagnostic_emitter.hpp"
#include "myc/lexer/token_factory.hpp"
#include "myc/lexer/token_flags.hpp"

namespace myc::lexer::scanner_utils {

namespace {

constexpr std::size_t kMaxOperatorLength = 4;

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

    const auto location =
        context.GetSourceManager().GetLocation(context.GetFileId().Value(), start);
    const auto span = MakeSpan(context, start, end);
    (void)context.GetDiagnostics().EmitError(diagnostics::ErrorCode{"MYC0001"},
                                             std::string(message));
    (void)location;
    (void)span;
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
