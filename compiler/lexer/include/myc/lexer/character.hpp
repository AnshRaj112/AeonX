#pragma once

#include "myc/lexer/future_unicode_support.hpp"
#include "myc/source/source_location.hpp"

#include <compare>
#include <cstddef>
#include <cstdint>

namespace myc::lexer {

/// A single decoded Unicode scalar value with source position metadata.
///
/// Characters are value types produced by CharacterStream. They carry byte-offset
/// positions compatible with SourceManager while exposing a Unicode code point.
class Character {
public:
    Character() = default;

    Character(char32_t unicode_value, std::size_t byte_offset, myc::LineNumber line,
              myc::ColumnNumber column, source::SourceLocation location,
              FutureUnicodeSupport unicode = {}) noexcept
        : unicode_value_(unicode_value),
          byte_offset_(byte_offset),
          line_(line),
          column_(column),
          location_(location),
          unicode_(unicode) {}

    [[nodiscard]] char32_t GetUnicodeValue() const noexcept { return unicode_value_; }
    [[nodiscard]] std::size_t GetByteOffset() const noexcept { return byte_offset_; }
    [[nodiscard]] myc::LineNumber GetLine() const noexcept { return line_; }
    [[nodiscard]] myc::ColumnNumber GetColumn() const noexcept { return column_; }
    [[nodiscard]] const source::SourceLocation& GetLocation() const noexcept { return location_; }
    [[nodiscard]] const FutureUnicodeSupport& GetUnicodeMetadata() const noexcept {
        return unicode_;
    }

    [[nodiscard]] bool IsEof() const noexcept { return unicode_value_ == 0; }
    [[nodiscard]] bool IsAscii() const noexcept {
        return unicode_value_ >= 0 && unicode_value_ <= 0x7F;
    }

    [[nodiscard]] bool operator==(const Character& other) const noexcept {
        return unicode_value_ == other.unicode_value_ && byte_offset_ == other.byte_offset_ &&
               line_ == other.line_ && column_ == other.column_ &&
               location_ == other.location_ && unicode_ == other.unicode_;
    }

private:
    char32_t unicode_value_{0};
    std::size_t byte_offset_{0};
    myc::LineNumber line_{1};
    myc::ColumnNumber column_{1};
    source::SourceLocation location_;
    FutureUnicodeSupport unicode_;
};

}  // namespace myc::lexer
