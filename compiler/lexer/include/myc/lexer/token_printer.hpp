#pragma once

#include "myc/lexer/token.hpp"

#include <ostream>
#include <string>

namespace myc::lexer {

/// Produces human-readable token representations for debugging and tooling.
class TokenPrinter {
public:
    [[nodiscard]] static std::string Print(const Token& token);
    [[nodiscard]] static std::string PrintType(const Token& token);
    [[nodiscard]] static std::string PrintWithLocation(const Token& token);

    static void PrintTo(std::ostream& output, const Token& token);
};

}  // namespace myc::lexer
