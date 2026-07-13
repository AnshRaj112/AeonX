#include "myc/lexer/character_stream.hpp"

#include "myc/lexer/utf8_decoder.hpp"

namespace myc::lexer {

CharacterStream::CharacterStream(CharacterReader reader) : reader_(reader) {}

Character CharacterStream::Peek(std::size_t offset) const {
    std::size_t byte_offset = position_;
    for (std::size_t i = 0; i < offset; ++i) {
        if (byte_offset >= reader_.GetSize()) {
            return DecodeAt(reader_.GetSize());
        }
        byte_offset += CodePointByteLength(byte_offset);
    }
    return DecodeAt(byte_offset);
}

Character CharacterStream::Previous() const {
    if (position_ == 0) {
        return Character{};
    }

    std::size_t scan = 0;
    std::size_t previous_start = 0;
    while (scan < position_) {
        previous_start = scan;
        scan += CodePointByteLength(scan);
    }
    return DecodeAt(previous_start);
}

Character CharacterStream::Advance() {
    if (Eof()) {
        return DecodeAt(reader_.GetSize());
    }

    const Character current = DecodeAt(position_);
    const std::size_t bytes = CodePointByteLength(position_);
    position_ += bytes;
    ++characters_read_;
    UpdateLineTracking(current);
    return current;
}

bool CharacterStream::Eof() const noexcept { return position_ >= reader_.GetSize(); }

void CharacterStream::Rewind(std::size_t code_points) {
    for (std::size_t i = 0; i < code_points && position_ > 0; ++i) {
        std::size_t scan = 0;
        std::size_t previous_start = 0;
        while (scan < position_) {
            previous_start = scan;
            scan += CodePointByteLength(scan);
        }
        position_ = previous_start;
        if (characters_read_ > 0) {
            --characters_read_;
        }
    }
}

void CharacterStream::Reset(std::size_t mark) {
    if (mark > reader_.GetSize()) {
        position_ = reader_.GetSize();
        return;
    }
    position_ = mark;
}

std::string_view CharacterStream::Slice(std::size_t start, std::size_t end) const {
    return reader_.Slice(start, end);
}

Character CharacterStream::DecodeAt(std::size_t byte_offset) const {
    if (byte_offset >= reader_.GetSize()) {
        const auto location = reader_.MakeLocation(reader_.GetSize());
        return Character{0, reader_.GetSize(), location.GetLine(), location.GetColumn(), location};
    }

    const auto decoded = DecodeUtf8(reader_.GetContent(), byte_offset);
    const auto location = reader_.MakeLocation(byte_offset);
    return Character{decoded.code_point, byte_offset, location.GetLine(), location.GetColumn(),
                     location};
}

std::size_t CharacterStream::CodePointByteLength(std::size_t byte_offset) const {
    if (byte_offset >= reader_.GetSize()) {
        return 0;
    }
    return DecodeUtf8(reader_.GetContent(), byte_offset).bytes_consumed;
}

void CharacterStream::UpdateLineTracking(const Character& consumed) {
    if (consumed.GetLine() > max_line_) {
        max_line_ = consumed.GetLine();
    }
    if (consumed.GetUnicodeValue() == U'\n') {
        const myc::LineNumber next_line = consumed.GetLine() + 1;
        if (next_line > max_line_) {
            max_line_ = next_line;
        }
    }
}

}  // namespace myc::lexer
