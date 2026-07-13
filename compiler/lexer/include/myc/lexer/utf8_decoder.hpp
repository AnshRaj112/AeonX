#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace myc::lexer {

/// Result of decoding one UTF-8 code point at a byte offset.
struct Utf8DecodeResult {
    char32_t code_point{0};
    std::size_t bytes_consumed{0};
    bool valid{false};
};

/// Decodes a single UTF-8 scalar value from text at the given byte offset.
///
/// On invalid input, returns U+FFFD and consumes one byte so scanning can recover.
[[nodiscard]] constexpr Utf8DecodeResult DecodeUtf8(std::string_view text,
                                                    std::size_t offset) noexcept {
    if (offset >= text.size()) {
        return {.code_point = 0, .bytes_consumed = 0, .valid = true};
    }

    const auto byte = static_cast<unsigned char>(text[offset]);

    if (byte <= 0x7F) {
        return {.code_point = static_cast<char32_t>(byte), .bytes_consumed = 1, .valid = true};
    }

    std::size_t expected = 0;
    char32_t code_point = 0;

    if ((byte & 0xE0) == 0xC0) {
        expected = 2;
        code_point = byte & 0x1F;
    } else if ((byte & 0xF0) == 0xE0) {
        expected = 3;
        code_point = byte & 0x0F;
    } else if ((byte & 0xF8) == 0xF0) {
        expected = 4;
        code_point = byte & 0x07;
    } else {
        return {.code_point = U'\uFFFD', .bytes_consumed = 1, .valid = false};
    }

    if (offset + expected > text.size()) {
        return {.code_point = U'\uFFFD', .bytes_consumed = 1, .valid = false};
    }

    for (std::size_t i = 1; i < expected; ++i) {
        const auto continuation = static_cast<unsigned char>(text[offset + i]);
        if ((continuation & 0xC0) != 0x80) {
            return {.code_point = U'\uFFFD', .bytes_consumed = 1, .valid = false};
        }
        code_point = (code_point << 6) | (continuation & 0x3F);
    }

    if ((expected == 2 && code_point < 0x80) || (expected == 3 && code_point < 0x800) ||
        (expected == 4 && code_point < 0x10000) || code_point > 0x10FFFF ||
        (code_point >= 0xD800 && code_point <= 0xDFFF)) {
        return {.code_point = U'\uFFFD', .bytes_consumed = 1, .valid = false};
    }

    return {.code_point = code_point, .bytes_consumed = expected, .valid = true};
}

}  // namespace myc::lexer
