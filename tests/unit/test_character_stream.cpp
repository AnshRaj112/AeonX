#include "myc_test.hpp"

#include "myc/lexer/character_reader.hpp"
#include "myc/lexer/character_stream.hpp"
#include "myc/lexer/utf8_decoder.hpp"
#include "myc/source/source_manager.hpp"

#include <string>

namespace {

myc::lexer::CharacterStream MakeStream(myc::source::SourceManager& manager,
                                       const std::string& content) {
    const myc::FileId id = manager.AddMemoryBuffer("stream.myc", content);
    const myc::source::SourceBuffer* buffer = manager.GetBuffer(id);
    MYC_ASSERT(buffer != nullptr);
    myc::lexer::CharacterReader reader(*buffer, manager);
    return myc::lexer::CharacterStream(reader);
}

}  // namespace

MYC_TEST(character_stream_peek_and_advance) {
    myc::source::SourceManager manager;
    myc::lexer::CharacterStream stream = MakeStream(manager, "abc");

    MYC_ASSERT(!stream.Eof());
    MYC_ASSERT_EQ(stream.Peek().GetUnicodeValue(), U'a');
    MYC_ASSERT_EQ(stream.Position(), 0u);

    const auto first = stream.Advance();
    MYC_ASSERT_EQ(first.GetUnicodeValue(), U'a');
    MYC_ASSERT_EQ(stream.Peek().GetUnicodeValue(), U'b');
    MYC_ASSERT_EQ(stream.Position(), 1u);
    MYC_ASSERT_EQ(stream.GetCharactersRead(), 1u);
}

MYC_TEST(character_stream_peek_offset) {
    myc::source::SourceManager manager;
    myc::lexer::CharacterStream stream = MakeStream(manager, "hello");

    MYC_ASSERT_EQ(stream.Peek(0).GetUnicodeValue(), U'h');
    MYC_ASSERT_EQ(stream.Peek(2).GetUnicodeValue(), U'l');
    MYC_ASSERT_EQ(stream.Position(), 0u);
}

MYC_TEST(character_stream_rewind_and_mark_reset) {
    myc::source::SourceManager manager;
    myc::lexer::CharacterStream stream = MakeStream(manager, "abcd");

    (void)stream.Advance();
    (void)stream.Advance();
    const std::size_t mark = stream.Mark();
    MYC_ASSERT_EQ(stream.Peek().GetUnicodeValue(), U'c');

    (void)stream.Advance();
    MYC_ASSERT_EQ(stream.Peek().GetUnicodeValue(), U'd');

    stream.Rewind(1);
    MYC_ASSERT_EQ(stream.Peek().GetUnicodeValue(), U'c');

    stream.Reset(mark);
    MYC_ASSERT_EQ(stream.Peek().GetUnicodeValue(), U'c');
    MYC_ASSERT_EQ(stream.Position(), 2u);
}

MYC_TEST(character_stream_eof) {
    myc::source::SourceManager manager;
    myc::lexer::CharacterStream stream = MakeStream(manager, "x");

    (void)stream.Advance();
    MYC_ASSERT(stream.Eof());
    MYC_ASSERT(stream.Peek().IsEof());
    MYC_ASSERT(stream.Current().IsEof());
}

MYC_TEST(character_stream_position_tracking) {
    myc::source::SourceManager manager;
    myc::lexer::CharacterStream stream = MakeStream(manager, "ab");

    MYC_ASSERT_EQ(stream.Previous().GetUnicodeValue(), 0);
    (void)stream.Advance();
    MYC_ASSERT_EQ(stream.Previous().GetUnicodeValue(), U'a');
    MYC_ASSERT_EQ(stream.Previous().GetByteOffset(), 0u);
}

MYC_TEST(character_stream_line_and_column_tracking) {
    myc::source::SourceManager manager;
    myc::lexer::CharacterStream stream = MakeStream(manager, "a\nbc");

    const auto first = stream.Advance();
    MYC_ASSERT_EQ(first.GetLine(), 1u);
    MYC_ASSERT_EQ(first.GetColumn(), 1u);

    const auto newline = stream.Advance();
    MYC_ASSERT_EQ(newline.GetUnicodeValue(), U'\n');

    const auto third = stream.Advance();
    MYC_ASSERT_EQ(third.GetLine(), 2u);
    MYC_ASSERT_EQ(third.GetColumn(), 1u);
    MYC_ASSERT_EQ(stream.GetMaxLine(), 2u);
}

MYC_TEST(character_stream_utf8_reading) {
    myc::source::SourceManager manager;
    myc::lexer::CharacterStream stream = MakeStream(manager, "caf\u00e9");

    MYC_ASSERT_EQ(stream.Advance().GetUnicodeValue(), U'c');
    MYC_ASSERT_EQ(stream.Advance().GetUnicodeValue(), U'a');
    MYC_ASSERT_EQ(stream.Advance().GetUnicodeValue(), U'f');
    const auto accented = stream.Advance();
    MYC_ASSERT_EQ(accented.GetUnicodeValue(), U'\u00e9');
    MYC_ASSERT_EQ(stream.Position(), 5u);
}

MYC_TEST(character_stream_utf8_decoder_invalid) {
    const std::string invalid = std::string("\xFF", 1);
    const auto decoded = myc::lexer::DecodeUtf8(invalid, 0);
    MYC_ASSERT(!decoded.valid);
    MYC_ASSERT_EQ(decoded.code_point, U'\uFFFD');
    MYC_ASSERT_EQ(decoded.bytes_consumed, 1u);
}

MYC_TEST(character_stream_slice) {
    myc::source::SourceManager manager;
    myc::lexer::CharacterStream stream = MakeStream(manager, "hello world");

    MYC_ASSERT_EQ(stream.Slice(0, 5), "hello");
    MYC_ASSERT_EQ(stream.Slice(6, 11), "world");
}

MYC_TEST(character_stream_next_alias) {
    myc::source::SourceManager manager;
    myc::lexer::CharacterStream stream = MakeStream(manager, "xy");

    MYC_ASSERT_EQ(stream.Next().GetUnicodeValue(), U'x');
    MYC_ASSERT_EQ(stream.Next().GetUnicodeValue(), U'y');
}
