#pragma once

#include "myc/lexer/token_category.hpp"
#include "myc/lexer/token_type.hpp"

namespace myc::lexer {

[[nodiscard]] TokenCategory GetCategory(TokenType type) noexcept;

}  // namespace myc::lexer
