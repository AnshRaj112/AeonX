#include "myc_test.hpp"

#include "myc/diagnostics/diagnostic_engine.hpp"
#include "myc/source/encoding.hpp"
#include "myc/source/line_table.hpp"
#include "myc/source/source_buffer.hpp"
#include "myc/source/source_manager.hpp"
#include "myc/source/source_span.hpp"

#include <filesystem>
#include <fstream>
#include <string>

namespace {

std::filesystem::path WriteTempFile(const std::string& name, const std::string& content) {
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / ("myc_source_test_" + name);
    std::ofstream file(path, std::ios::binary);
    file << content;
    file.close();
    return path;
}

void RemoveTempFile(const std::filesystem::path& path) {
    std::error_code ec;
    std::filesystem::remove(path, ec);
}

myc::source::SourceBuffer MakeBuffer(myc::FileId id, std::string path, std::string content) {
    myc::source::LineTable table(content);
    const bool valid = myc::source::EncodingDetector::IsValidUtf8(content);
    return myc::source::SourceBuffer(myc::source::SourceFileID(id), std::move(path),
                                     std::move(content), std::move(table), valid);
}

}  // namespace

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
}

MYC_TEST(source_buffer_line_lookup) {
    myc::source::SourceBuffer buffer = MakeBuffer(1, "lines.myc", "a\nbc\ndef\n");
    MYC_ASSERT_EQ(buffer.GetLineCount(), 4u);
    const auto [line, column] = buffer.GetLineAndColumn(3);
    MYC_ASSERT_EQ(line, 2u);
    MYC_ASSERT_EQ(column, 2u);
}

MYC_TEST(source_buffer_slices_and_access) {
    myc::source::SourceBuffer buffer = MakeBuffer(1, "slice.myc", "hello world");
    MYC_ASSERT_EQ(buffer.At(0), 'h');
    MYC_ASSERT_EQ(buffer.TryAt(100), std::nullopt);
    MYC_ASSERT_EQ(buffer.Substring(0, 5), "hello");
    MYC_ASSERT_EQ(buffer.Slice(6, 11), "world");
}

MYC_TEST(source_file_load_utf8) {
    const auto path = WriteTempFile("utf8.myc", "let x = 42;\n");
    myc::source::SourceManager manager;
    const auto id = manager.LoadFile(path);
    MYC_ASSERT(id.has_value());
    const myc::source::SourceFile* file = manager.GetFile(*id);
    MYC_ASSERT(file != nullptr);
    MYC_ASSERT(file->GetEncoding() == myc::source::SourceEncoding::UTF8);
    MYC_ASSERT(file->GetBuffer().IsValidUtf8());
    RemoveTempFile(path);
}

MYC_TEST(source_file_load_crlf) {
    const auto path = WriteTempFile("crlf.myc", "line1\r\nline2\r\n");
    myc::source::SourceManager manager;
    const auto id = manager.LoadFile(path);
    MYC_ASSERT(id.has_value());
    const auto* buffer = manager.GetBuffer(*id);
    MYC_ASSERT_EQ(buffer->GetLineCount(), 3u);
    MYC_ASSERT_EQ(buffer->GetLine(1), "line1");
    MYC_ASSERT_EQ(buffer->GetLine(2), "line2");
    RemoveTempFile(path);
}

MYC_TEST(source_file_load_empty) {
    const auto path = WriteTempFile("empty.myc", "");
    myc::source::SourceManager manager;
    const auto id = manager.LoadFile(path);
    MYC_ASSERT(id.has_value());
    const auto* buffer = manager.GetBuffer(*id);
    MYC_ASSERT_EQ(buffer->GetSize(), 0u);
    MYC_ASSERT_EQ(buffer->GetLineCount(), 1u);
    RemoveTempFile(path);
}

MYC_TEST(source_file_load_large) {
    std::string content;
    content.reserve(256 * 1024);
    for (int i = 0; i < 16 * 1024; ++i) {
        content += "line ";
        content += std::to_string(i);
        content += '\n';
    }
    const auto path = WriteTempFile("large.myc", content);
    myc::source::SourceManager manager;
    const auto id = manager.LoadFile(path);
    MYC_ASSERT(id.has_value());
    const auto* buffer = manager.GetBuffer(*id);
    MYC_ASSERT_EQ(buffer->GetSize(), content.size());
    MYC_ASSERT_EQ(buffer->GetLineCount(), 16 * 1024 + 1);
    const auto [line, column] = buffer->GetLineAndColumn(content.size() / 2);
    MYC_ASSERT(line > 1u);
    MYC_ASSERT(column >= 1u);
    RemoveTempFile(path);
}

MYC_TEST(source_file_invalid_path) {
    myc::diagnostics::DiagnosticEngine engine;
    myc::source::SourceManager manager;
    manager.SetDiagnosticsEngine(&engine);
    const auto id = manager.LoadFile("definitely_missing_file_12345.myc");
    MYC_ASSERT(!id.has_value());
    MYC_ASSERT(engine.HasErrors());
}

MYC_TEST(source_file_invalid_encoding) {
    const std::string invalid = std::string("\xFF\xFE", 2);
    const auto path = WriteTempFile("invalid.myc", invalid);
    myc::diagnostics::DiagnosticEngine engine;
    myc::source::SourceManager manager;
    manager.SetDiagnosticsEngine(&engine);
    const auto id = manager.LoadFile(path);
    MYC_ASSERT(!id.has_value());
    MYC_ASSERT(engine.HasErrors());
    RemoveTempFile(path);
}

MYC_TEST(source_file_duplicate_loading) {
    const auto path = WriteTempFile("dup.myc", "fn main() {}\n");
    myc::source::SourceManager manager;
    const auto first = manager.LoadFile(path);
    const auto second = manager.LoadFile(path);
    MYC_ASSERT(first.has_value());
    MYC_ASSERT(second.has_value());
    MYC_ASSERT_EQ(*first, *second);
    MYC_ASSERT_EQ(manager.GetFileCount(), 1u);
    RemoveTempFile(path);
}

MYC_TEST(source_location_and_span) {
    myc::source::SourceManager manager;
    const myc::FileId id = manager.AddMemoryBuffer("span.myc", "abcdef");
    const myc::source::SourceLocation start = manager.GetLocation(id, 1);
    const myc::source::SourceLocation end = manager.GetLocation(id, 4);
    MYC_ASSERT(start.IsValid());
    MYC_ASSERT_EQ(start.GetOffset(), 1u);

    const myc::source::SourceSpan span(start, end);
    MYC_ASSERT(span.IsValid());
    MYC_ASSERT_EQ(span.GetLength(), 3u);
    MYC_ASSERT(span.Contains(manager.GetLocation(id, 2)));
    MYC_ASSERT(!span.Contains(manager.GetLocation(id, 4)));

    const auto merged = span.Merge(manager.MakeSpan(id, 4, 6));
    MYC_ASSERT(merged.has_value());
    MYC_ASSERT_EQ(merged->GetLength(), 5u);
}

MYC_TEST(source_utf8_bom_stripped) {
    const std::string with_bom = std::string("\xEF\xBB\xBF", 3) + "bom\n";
    const auto path = WriteTempFile("bom.myc", with_bom);
    myc::source::SourceManager manager;
    const auto id = manager.LoadFile(path);
    MYC_ASSERT(id.has_value());
    const auto* buffer = manager.GetBuffer(*id);
    MYC_ASSERT_EQ(buffer->GetContent(), "bom\n");
    RemoveTempFile(path);
}

MYC_TEST(source_virtual_file) {
    myc::source::VirtualFile virtual_info;
    virtual_info.kind = myc::source::VirtualFileKind::ReplInput;
    virtual_info.display_name = "repl";
    virtual_info.is_unsaved = true;

    myc::source::SourceManager manager;
    const myc::FileId id = manager.AddVirtualFile(virtual_info, "1 + 2\n");
    const myc::source::SourceFile* file = manager.GetFile(id);
    MYC_ASSERT(file != nullptr);
    MYC_ASSERT(file->GetKind() == myc::source::VirtualFileKind::ReplInput);
}

MYC_TEST(encoding_detector) {
    MYC_ASSERT(myc::source::EncodingDetector::IsValidUtf8("hello"));
    MYC_ASSERT(!myc::source::EncodingDetector::IsValidUtf8("\xC0\x80"));
    MYC_ASSERT_EQ(myc::source::EncodingDetector::BomLength("\xEF\xBB\xBFx"), 3u);
}

MYC_TEST(line_table_column_mapping) {
    myc::source::LineTable table("ab\ncd\n");
    const auto [line, column] = table.GetLineAndColumn(3);
    MYC_ASSERT_EQ(line, 2u);
    MYC_ASSERT_EQ(column, 1u);
    MYC_ASSERT_EQ(table.GetLineStartOffset(2), 3u);
}

MYC_TEST(source_span_intersects) {
    myc::source::SourceLocation a(myc::source::SourceFileID(1), 0, 1, 1);
    myc::source::SourceLocation b(myc::source::SourceFileID(1), 5, 1, 6);
    myc::source::SourceLocation c(myc::source::SourceFileID(1), 3, 1, 4);
    myc::source::SourceLocation d(myc::source::SourceFileID(1), 8, 1, 9);

    const myc::source::SourceSpan left(a, b);
    const myc::source::SourceSpan overlapping(c, d);
    MYC_ASSERT(left.Intersects(overlapping));
    MYC_ASSERT(!left.Intersects(myc::source::SourceSpan(d, d)));
}

MYC_TEST(source_file_metadata) {
    const auto path = WriteTempFile("meta.myc", "content");
    myc::source::SourceManager manager;
    const auto id = manager.LoadFile(path);
    MYC_ASSERT(id.has_value());
    const myc::source::SourceFile* file = manager.GetFile(*id);
    MYC_ASSERT(!file->GetFilename().empty());
    MYC_ASSERT(file->GetExtension() == ".myc");
    MYC_ASSERT(manager.IsLoaded(path));
    MYC_ASSERT(manager.FindFileByPath(path).has_value());
    RemoveTempFile(path);
}
