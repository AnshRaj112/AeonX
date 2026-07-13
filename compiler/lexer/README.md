# Lexer Module — Production Lexical Analysis

The lexer module converts Myc source text into an immutable `TokenStream`. It
includes a complete character streaming layer, modular scanner architecture,
keyword/operator lookup tables, error recovery, and statistics collection.

## Architecture

```
SourceBuffer
     │
     ▼
CharacterReader ──► CharacterStream ──► Character
     │                      │
     │                      ▼
SourceManager          Lexer (coordinator)
     │                      │
     │                      ├── LexerContext
     │                      ├── LexerConfiguration / LexerOptions
     │                      ├── LexerStatistics
     │                      └── ScannerRegistry
     │                              │
     │                              ▼
     │                    IScanner implementations
     │                    (one per lexical category)
     │                              │
     ▼                              ▼
DiagnosticEmitter            TokenFactory ──► Token ──► TokenStream
```

## Tokenization algorithm

1. Construct `Lexer` over a `SourceBuffer`, `SourceManager`, and `DiagnosticEmitter`.
2. Build a `LexerContext` shared with all scanners.
3. Loop until end-of-input:
   - Dispatch scanners in priority order via `ScannerRegistry`.
   - `TokenProduced` → flush pending tokens into the output vector.
   - `Consumed` → continue (whitespace/comments skipped by default).
   - `Error` → flush recoverable error tokens and continue when configured.
   - `NotHandled` from all scanners → advance one code point and emit a recoverable error.
4. Append a terminating `EndOfFile` token.
5. Return `LexerResult` containing `TokenStream`, `LexerStatistics`, and `had_errors`.

Lexemes are always zero-copy `std::string_view` slices into the `SourceBuffer`.

## Scanner responsibilities

Scanners implement `IScanner::Scan(LexerContext&)` and return `ScanResult`.

| Scanner | Responsibility |
|---------|----------------|
| `WhitespaceScanner` | Spaces, tabs, CR/LF; skipped unless trivia mode is enabled |
| `CommentScanner` | `//` line comments, `/* */` block comments (nested blocks supported) |
| `DirectiveScanner` | `#include`, `#define`, `#if`, `#else`, `#endif`, `#pragma` lines |
| `AnnotationScanner` | `@name` attribute tokens |
| `StringScanner` | `"..."` literals with escape sequences |
| `CharacterScanner` | `'...'` character literals with escapes |
| `NumberScanner` | Decimal, `0x` hex, `0b` binary, `0o` octal, float, scientific |
| `KeywordScanner` | Reserved words and primitive types via `KeywordTable` |
| `IdentifierScanner` | ASCII/Unicode-placeholder identifiers |
| `OperatorScanner` | Longest-match operators, delimiters, punctuation |
| `UnknownScanner` | Final fallback; emits recoverable `Error` tokens |

### Dispatch order

1. Whitespace
2. Comment
3. Directive
4. Annotation
5. String
6. Character
7. Number
8. Keyword
9. Identifier
10. Operator
11. Unknown

Keyword lookup runs before identifier scanning so reserved words are never
classified as identifiers.

## Lookup tables

### KeywordTable

Centralized sorted keyword database (`keyword.cpp`). Lookup is `O(log n)` binary
search. Primitive types (`bool`, `int`, `string`, …) and keywords (`if`, `fn`,
`struct`, …) share the same table.

Future keywords are added by appending a sorted `MakeKeyword(...)` entry and
relying on the compile-time `static_assert` sort check.

### OperatorTable

Centralized sorted operator database (`operator.cpp`) with precedence,
associativity, and arity metadata. `OperatorScanner` performs longest-match
classification using the table (e.g. `>=` is never split into `>` and `=`).

## Error recovery

The lexer **never stops after the first error** when `continue_after_errors` is
enabled (default).

Recoverable situations:

- Unknown characters
- Invalid escape sequences
- Unterminated strings or character literals
- Invalid numeric literals
- Unterminated block comments

Each produces a `TokenType::Error` token with `TokenFlags::Recoverable` and a
diagnostic via `DiagnosticEmitter`, then scanning continues.

## Configuration

`LexerOptions` → `LexerConfiguration`:

| Option | Default | Effect |
|--------|---------|--------|
| `collect_statistics` | `true` | Track characters, tokens, timing |
| `continue_after_errors` | `true` | Keep scanning after lexical errors |
| `skip_whitespace` | `false` | Reserved for future trivia control |
| `include_trivia` | `false` | Emit `Whitespace`, `Newline`, `Comment` tokens |
| `enable_directives` | `true` | Recognize `#directive` lines |
| `enable_annotations` | `true` | Recognize `@annotation` tokens |

## Statistics

`LexerStatistics` collects:

- Characters read
- Lines processed
- Scanner invocations
- Tokens produced
- Errors encountered
- Elapsed wall time (`std::chrono::steady_clock`)
- Memory usage estimate (token vector capacity)

## Future Unicode support

`Character` carries a `FutureUnicodeSupport` placeholder. Identifiers currently
accept ASCII letters, digits, underscore, and a non-ASCII Unicode placeholder
path for future identifier rules. `utf8_decoder.hpp` replaces invalid UTF-8 with
U+FFFD and advances one byte so scanning can continue.

## Incremental lexing

`FutureIncrementalLexing` on `LexerConfiguration` reserves fields for IDE
re-lexing of edited regions without re-tokenizing entire files.

## Performance considerations

- **Zero-copy lexemes:** tokens slice `SourceBuffer` text directly
- **Precomputed line table:** `SourceManager` resolves locations in `O(log n)`
- **Sorted lookup tables:** keywords and operators use cache-friendly binary search
- **Dedicated scanners:** no monolithic switch; each category is isolated
- **Minimal allocation:** cursor-only `CharacterStream`; token vector reserved at start
- **Streaming input:** `CharacterReader` is a non-owning view over mapped/loaded text

## Building and testing

```bash
cmake --build build --target myc_lexer
cmake --build build --target myc_tests
ctest --test-dir build -R myc_unit_tests --output-on-failure
```

Lexer-focused tests:

| File | Coverage |
|------|----------|
| `tests/unit/test_lexer_production.cpp` | 280+ production lexer scenarios |
| `tests/unit/test_lexer_infrastructure.cpp` | Coordinator, statistics, recovery |
| `tests/unit/test_lexer.cpp` | Token system, tables, `TokenStream` |
| `tests/unit/test_lexer_debug.cpp` | Lookup table regression guards |
| `tests/unit/test_character_stream.cpp` | UTF-8 streaming layer |

## Status

| Component | Status |
|-----------|--------|
| Token system | Implemented |
| CharacterStream | Implemented |
| Lexer coordinator | Implemented |
| All 11 scanners | Implemented |
| Keyword/operator tables | Implemented (sorted, `static_assert` verified) |
| Error recovery | Implemented |
| Statistics | Implemented |
| Incremental lexing | Placeholder |
| Raw/multiline strings | Placeholder (`NotHandled`) |
