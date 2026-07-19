#pragma once

#include "myc/ast/ast_node.hpp"
#include "myc/diagnostics/diagnostic_emitter.hpp"
#include "myc/lexer/token.hpp"
#include "myc/parser/parser_configuration.hpp"
#include "myc/parser/parser_state.hpp"
#include "myc/parser/parser_statistics.hpp"
#include "myc/parser/token_cursor.hpp"
#include "myc/source/source_manager.hpp"

#include <memory>
#include <string>
#include <string_view>

namespace myc::parser {

class RecoveryManager;

/// Shared parsing environment passed to specialized parser components.
class ParserContext {
public:
    ParserContext(lexer::TokenStream tokens, source::SourceManager& source_manager,
                  diagnostics::DiagnosticEmitter& diagnostics,
                  ParserConfiguration configuration = {});

    ~ParserContext();

    [[nodiscard]] TokenCursor& GetCursor() noexcept { return cursor_; }
    [[nodiscard]] const TokenCursor& GetCursor() const noexcept { return cursor_; }

    [[nodiscard]] ParserState& GetState() noexcept { return state_; }
    [[nodiscard]] const ParserState& GetState() const noexcept { return state_; }

    [[nodiscard]] ParserStatistics& GetStatistics() noexcept { return statistics_; }
    [[nodiscard]] const ParserStatistics& GetStatistics() const noexcept { return statistics_; }

    [[nodiscard]] ParserConfiguration& GetConfiguration() noexcept { return configuration_; }
    [[nodiscard]] const ParserConfiguration& GetConfiguration() const noexcept {
        return configuration_;
    }

    [[nodiscard]] source::SourceManager& GetSourceManager() noexcept { return source_manager_; }
    [[nodiscard]] diagnostics::DiagnosticEmitter& GetDiagnostics() noexcept { return diagnostics_; }
    [[nodiscard]] RecoveryManager& GetRecovery() noexcept { return *recovery_; }

    [[nodiscard]] const lexer::Token& Peek(std::size_t offset = 0) const;
    [[nodiscard]] const lexer::Token& LookAhead(std::size_t count) const;
    [[nodiscard]] const lexer::Token& Consume();
    [[nodiscard]] const lexer::Token& Previous() const;

    [[nodiscard]] bool Match(lexer::TokenType type);
    [[nodiscard]] bool MatchOneOf(std::initializer_list<lexer::TokenType> types);
    [[nodiscard]] bool Expect(lexer::TokenType type, std::string_view message);

    [[nodiscard]] std::size_t Checkpoint() const noexcept;
    void Rewind(std::size_t checkpoint) noexcept;

    [[nodiscard]] bool IsAtEnd() const noexcept;
    [[nodiscard]] bool IsDone() const noexcept;

    void ReportError(const lexer::Token& token, std::string_view message);
    void ReportErrorAt(source::SourceSpan span, std::string_view message);
    void Recover();

    template <typename NodeType, typename... Args>
    std::unique_ptr<NodeType> MakeNode(Args&&... args) {
        statistics_.RecordNodeCreated();
        return std::make_unique<NodeType>(std::forward<Args>(args)...);
    }

    void RecordDeclaration() noexcept { statistics_.RecordDeclarationParsed(); }
    void RecordStatement() noexcept { statistics_.RecordStatementParsed(); }
    void RecordExpression() noexcept { statistics_.RecordExpressionParsed(); }

    [[nodiscard]] diagnostics::SourceSpan ToDiagnosticSpan(const source::SourceSpan& span) const;
    [[nodiscard]] diagnostics::SourceSpan ToDiagnosticSpan(const lexer::Token& token) const;

private:
    TokenCursor cursor_;
    ParserState state_;
    ParserStatistics statistics_;
    ParserConfiguration configuration_;
    source::SourceManager& source_manager_;
    diagnostics::DiagnosticEmitter& diagnostics_;
    std::unique_ptr<RecoveryManager> recovery_;
};

}  // namespace myc::parser
