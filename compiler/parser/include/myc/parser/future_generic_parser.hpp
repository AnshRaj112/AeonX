#pragma once

#include "myc/parser/parser_context.hpp"

namespace myc::parser {

/// Placeholder for future generic type parser extensions.
class FutureGenericParser {
public:
    explicit FutureGenericParser(ParserContext& context);

    [[nodiscard]] bool IsGenericStart() const;

private:
    ParserContext& context_;
};

}  // namespace myc::parser
