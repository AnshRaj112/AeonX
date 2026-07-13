#include "myc/lexer/character_reader.hpp"

namespace myc::lexer {

CharacterReader::CharacterReader(const source::SourceBuffer& buffer,
                                 source::SourceManager& source_manager)
    : buffer_(buffer), source_manager_(source_manager) {}

std::string_view CharacterReader::GetContent() const noexcept { return buffer_.GetContent(); }

std::size_t CharacterReader::GetSize() const noexcept { return buffer_.GetSize(); }

bool CharacterReader::IsValidUtf8() const noexcept { return buffer_.IsValidUtf8(); }

char CharacterReader::At(std::size_t offset) const noexcept {
    if (const auto byte = buffer_.TryAt(offset)) {
        return *byte;
    }
    return '\0';
}

std::string_view CharacterReader::Slice(std::size_t start, std::size_t end) const {
    if (start >= end || end > buffer_.GetSize()) {
        return {};
    }
    return buffer_.Slice(start, end);
}

source::SourceLocation CharacterReader::MakeLocation(std::size_t offset) const {
    return source_manager_.GetLocation(buffer_.GetFileId(), offset);
}

}  // namespace myc::lexer
