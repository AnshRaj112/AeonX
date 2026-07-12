#pragma once

namespace myc::parser {

/// Placeholder interface for the parser subsystem.
class ParserModule {
public:
    [[nodiscard]] static const char* Name() noexcept;
};

}  // namespace myc::parser
