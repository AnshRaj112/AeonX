#pragma once

#include "myc/parser/parser_context.hpp"

namespace myc::parser {

/// Placeholder for future pattern-matching parser extensions.
class FuturePatternParser {
public:
    explicit FuturePatternParser(ParserContext& context);

    [[nodiscard]] bool IsPatternStart() const;

private:
    ParserContext& context_;
};

}  // namespace myc::parser
