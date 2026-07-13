#pragma once

#include "myc/source/source_buffer.hpp"
#include "myc/source/source_location.hpp"
#include "myc/source/source_manager.hpp"

#include <cstddef>
#include <string_view>

namespace myc::lexer {

/// Low-level read-only access to source text for character streaming.
///
/// CharacterReader separates raw buffer access from cursor management. It does not
/// produce tokens and performs no lexical classification.
class CharacterReader {
public:
    CharacterReader(const source::SourceBuffer& buffer, source::SourceManager& source_manager);

    [[nodiscard]] const source::SourceBuffer& GetBuffer() const noexcept { return buffer_; }
    [[nodiscard]] source::SourceManager& GetSourceManager() noexcept { return source_manager_; }
    [[nodiscard]] const source::SourceManager& GetSourceManager() const noexcept {
        return source_manager_;
    }

    [[nodiscard]] std::string_view GetContent() const noexcept;
    [[nodiscard]] std::size_t GetSize() const noexcept;
    [[nodiscard]] bool IsValidUtf8() const noexcept;

    /// Returns the raw byte at offset, or '\0' when out of range.
    [[nodiscard]] char At(std::size_t offset) const noexcept;

    /// Returns a half-open byte slice [start, end).
    [[nodiscard]] std::string_view Slice(std::size_t start, std::size_t end) const;

    /// Builds a SourceLocation for the byte offset via SourceManager.
    [[nodiscard]] source::SourceLocation MakeLocation(std::size_t offset) const;

private:
    const source::SourceBuffer& buffer_;
    source::SourceManager& source_manager_;
};

}  // namespace myc::lexer
