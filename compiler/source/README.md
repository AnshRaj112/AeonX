# Source Management Module

The source management subsystem provides Clang-style infrastructure for loading,
caching, and locating positions within Myc source files. It is the foundation for
the lexer, parser, diagnostics formatter, language server, formatter, linter,
and debugger.

## Architecture

```
FileLoader ──► SourceFile ──► SourceBuffer
                  │               │
                  │          LineTable
                  │
            SourceManager (cache, IDs, metadata)
                  │
     FutureIncludeResolver (placeholder)
```

### Core types

| Type | Responsibility |
|------|----------------|
| `SourceManager` | Central registry; loads files, assigns IDs, deduplicates by path |
| `SourceFile` | Immutable metadata + owned `SourceBuffer` |
| `SourceBuffer` | Read-only text access, slices, offset/line lookup |
| `SourceLocation` | File ID + byte offset + line/column |
| `SourceSpan` | Half-open byte range with containment/intersection/merge |
| `SourceFileID` | Strong wrapper around `FileId` |
| `LineTable` | Precomputed line starts for O(log n) mapping |
| `FileLoader` | Disk I/O, path normalization, encoding validation |
| `EncodingDetector` | UTF-8 validation and BOM handling |
| `VirtualFile` | Metadata for REPL, IDE, generated, and macro sources |
| `FutureIncludeResolver` | Placeholder for `#include` / `import` resolution |

## Responsibilities

- Load physical and in-memory source files
- Assign stable file identifiers starting at 1
- Cache loaded files by canonical absolute path
- Provide immutable, thread-safe read access after load
- Map byte offsets to line/column and back
- Validate UTF-8 and reject invalid encodings
- Emit diagnostics on load failures (never print directly)
- Reserve extension points for modules, packages, virtual files, and mmap

## Design rationale

**Immutable buffers.** Source text is copied once at load time and never mutated.
Downstream phases can read concurrently without synchronization.

**Precomputed line table.** Line starts are built once per file. Offset-to-line
lookup uses binary search instead of scanning on every diagnostic.

**Canonical path cache.** `weakly_canonical` absolute paths prevent duplicate
loads when the same file is referenced through different relative paths.

**Separation of concerns.** `FileLoader` handles I/O errors; `SourceManager`
handles caching and diagnostics; `SourceBuffer` handles text access. This keeps
each class focused and testable.

**Diagnostics integration.** `SourceManager::SetDiagnosticsEngine` wires load
failures into the shared `DiagnosticEngine`. Error codes live in the `MYC800x`
range (`SourceFileNotFound`, `SourceFileReadError`, `SourceInvalidEncoding`).

## Thread safety

Load operations take an exclusive lock. Read operations (`GetFile`, `GetBuffer`,
`GetLocation`) take a shared lock. Loaded `SourceFile` and `SourceBuffer`
objects are immutable, so concurrent reads are safe.

## Future compiler integration

```cpp
myc::source::SourceManager source;
source.SetDiagnosticsEngine(&engine);

const auto file_id = source.LoadFile("main.myc");
if (!file_id) {
    return engine.GetExitCode();
}

const myc::source::SourceBuffer* buffer = source.GetBuffer(*file_id);
const auto location = source.GetLocation(*file_id, byte_offset);
const auto span = source.MakeSpan(*file_id, start, end);

// Pass to lexer (future):
// Lexer lexer(*buffer, location);
```

### Planned extensions

- `FutureIncludeResolver` — module and package import paths
- `VirtualFile` — unsaved IDE buffers, REPL cells, macro expansions
- `content_hash` / `content_checksum` — incremental compilation
- Memory-mapped files for very large inputs

## Examples

### Load a file with diagnostics

```cpp
myc::diagnostics::DiagnosticEngine engine;
myc::source::SourceManager manager;
manager.SetDiagnosticsEngine(&engine);

if (const auto id = manager.LoadFile("src/main.myc")) {
    const auto* file = manager.GetFile(*id);
    std::cout << file->GetFilename() << ": " << file->GetBuffer().GetLineCount() << " lines\n";
}
```

### In-memory REPL buffer

```cpp
myc::source::VirtualFile repl;
repl.kind = myc::source::VirtualFileKind::ReplInput;
repl.display_name = "<repl>";
repl.is_unsaved = true;

const myc::FileId id = manager.AddVirtualFile(repl, user_input);
```

### Source spans

```cpp
const auto start = manager.GetLocation(file_id, 10);
const auto end = manager.GetLocation(file_id, 20);
myc::source::SourceSpan span(start, end);

if (span.Contains(other_offset)) { /* ... */ }
const auto merged = span.Merge(other_span);
```

## Error codes

| Code | Meaning |
|------|---------|
| `MYC8001` | Source file not found |
| `MYC8002` | Source file read error |
| `MYC8003` | Invalid UTF-8 encoding |
| `MYC8004` | Invalid source path (reserved) |

## Tests

Unit tests in `tests/unit/test_source.cpp` cover file loading, UTF-8, BOM,
line endings, line/column mapping, large and empty files, invalid paths,
duplicate loading, cache behavior, spans, locations, and virtual files.

Run:

```bash
cmake --build build --target myc_tests
ctest --test-dir build -R myc_unit_tests
```
