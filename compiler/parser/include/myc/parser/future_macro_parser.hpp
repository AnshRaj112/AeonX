#pragma once

#include "myc/parser/parser_context.hpp"

namespace myc::parser {

/// Placeholder for future macro expansion parser extensions.
class FutureMacroParser {
public:
    explicit FutureMacroParser(ParserContext& context);

    [[nodiscard]] bool IsMacroInvocation() const;

private:
    ParserContext& context_;
};

}  // namespace myc::parser
