#pragma once

#include <cstdint>

namespace myc::lexer {

/// Lexer mode controlling which scanner dispatch path is active.
enum class LexerState : std::uint8_t {
    Normal,
    InsideString,
    InsideComment,
    InsideDirective,
    FutureInterpolation,
    FutureRawString,
};

[[nodiscard]] constexpr const char* ToString(LexerState state) noexcept {
    switch (state) {
        case LexerState::Normal:
            return "Normal";
        case LexerState::InsideString:
            return "InsideString";
        case LexerState::InsideComment:
            return "InsideComment";
        case LexerState::InsideDirective:
            return "InsideDirective";
        case LexerState::FutureInterpolation:
            return "FutureInterpolation";
        case LexerState::FutureRawString:
            return "FutureRawString";
    }
    return "Unknown";
}

}  // namespace myc::lexer
