#include "myc_test.hpp"

#include "myc/source/source_manager.hpp"

MYC_TEST(source_manager_memory_buffer) {
    myc::source::SourceManager manager;
    const myc::FileId id = manager.AddMemoryBuffer("test.myc", "fn main() {}\n");
    MYC_ASSERT_EQ(id, 1u);
    MYC_ASSERT_EQ(manager.GetBufferCount(), 1u);

    const myc::source::SourceBuffer* buffer = manager.GetBuffer(id);
    MYC_ASSERT(buffer != nullptr);
    MYC_ASSERT(buffer->IsValidUtf8());
    MYC_ASSERT_EQ(buffer->GetLineCount(), 2u);

    const auto [line, column] = buffer->GetLineAndColumn(12);
    MYC_ASSERT_EQ(line, 1u);
    MYC_ASSERT_EQ(column, 13u);

    MYC_ASSERT_EQ(buffer->GetLineCount(), 2u);
}

MYC_TEST(source_buffer_line_lookup) {
    myc::source::SourceBuffer buffer(1, "lines.myc", "a\nbc\ndef\n");
    MYC_ASSERT_EQ(buffer.GetLineCount(), 4u);
    const auto [line, column] = buffer.GetLineAndColumn(3);
    MYC_ASSERT_EQ(line, 2u);
    MYC_ASSERT_EQ(column, 2u);
}
