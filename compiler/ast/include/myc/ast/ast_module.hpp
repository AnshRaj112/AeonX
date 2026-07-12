#pragma once

namespace myc::ast {

/// Placeholder interface for the AST subsystem.
class AstModule {
public:
    [[nodiscard]] static const char* Name() noexcept;
};

}  // namespace myc::ast
