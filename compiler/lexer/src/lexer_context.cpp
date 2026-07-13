#include "myc/lexer/lexer_context.hpp"

namespace myc::lexer {

LexerContext::LexerContext(CharacterStream& stream, source::SourceManager& source_manager,
                           diagnostics::DiagnosticEmitter& diagnostics,
                           LexerConfiguration configuration, LexerStatistics& statistics)
    : stream_(stream),
      source_manager_(source_manager),
      diagnostics_(diagnostics),
      configuration_(configuration),
      statistics_(statistics) {}

void LexerContext::PushToken(Token token) {
    pending_tokens_.push_back(std::move(token));
    statistics_.RecordTokenProduced();
}

source::SourceFileID LexerContext::GetFileId() const noexcept {
    return stream_.GetReader().GetBuffer().GetId();
}

}  // namespace myc::lexer
