#pragma once

#include "myc/lexer/character.hpp"
#include "myc/lexer/character_reader.hpp"

#include <cstddef>
#include <string_view>

namespace myc::lexer {

/// Streaming UTF-8 character cursor over a source buffer.
///
/// CharacterStream is responsible for character-level navigation only. It does not
/// produce tokens. Line and column metadata are resolved through SourceManager.
class CharacterStream {
public:
    explicit CharacterStream(CharacterReader reader);

    [[nodiscard]] const CharacterReader& GetReader() const noexcept { return reader_; }

    /// Peeks the character at the current position plus code-point offset.
    [[nodiscard]] Character Peek(std::size_t offset = 0) const;

    [[nodiscard]] Character Current() const { return Peek(0); }

    /// Returns the character immediately before the current position.
    [[nodiscard]] Character Previous() const;

    /// Consumes and returns the current character, advancing the cursor.
    [[nodiscard]] Character Advance();

    /// Alias for Advance().
    [[nodiscard]] Character Next() { return Advance(); }

    [[nodiscard]] bool Eof() const noexcept;

    /// Current byte offset of the cursor.
    [[nodiscard]] std::size_t Position() const noexcept { return position_; }

    /// Moves the cursor backward by the given number of code points.
    void Rewind(std::size_t code_points = 1);

    /// Records the current byte offset for later reset().
    [[nodiscard]] std::size_t Mark() const noexcept { return position_; }

    /// Restores the cursor to a previously recorded mark.
    void Reset(std::size_t mark);

    /// Returns a half-open byte slice [start, end) from the underlying buffer.
    [[nodiscard]] std::string_view Slice(std::size_t start, std::size_t end) const;

    /// Total number of code points consumed since construction or the last full reset.
    [[nodiscard]] std::size_t GetCharactersRead() const noexcept { return characters_read_; }

    /// Highest 1-based line number observed while scanning.
    [[nodiscard]] myc::LineNumber GetMaxLine() const noexcept { return max_line_; }

private:
    [[nodiscard]] Character DecodeAt(std::size_t byte_offset) const;
    [[nodiscard]] std::size_t CodePointByteLength(std::size_t byte_offset) const;
    void UpdateLineTracking(const Character& consumed);

    CharacterReader reader_;
    std::size_t position_{0};
    std::size_t characters_read_{0};
    myc::LineNumber max_line_{1};
};

}  // namespace myc::lexer
