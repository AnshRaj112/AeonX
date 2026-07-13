# Lexer Module — Character Stream and Lexer Infrastructure

The lexer module provides the character streaming layer and modular lexer framework
for the Myc compiler. Token data structures (`Token`, `TokenStream`, `TokenFactory`,
keyword/operator tables) are fully implemented; **lexical classification is not**.

This document covers the infrastructure beneath token generation.

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
     │                    (placeholders today)
     │                              │
     ▼                              ▼
DiagnosticEmitter            TokenFactory ──► Token ──► TokenStream
```

## CharacterStream

`CharacterStream` reads Unicode scalar values from a `SourceBuffer` without producing
tokens. It delegates location resolution to `SourceManager` via `CharacterReader`.

### API

| Method | Description |
|--------|-------------|
| `Peek(offset)` | Look ahead by code-point offset (0 = current) |
| `Current()` | Alias for `Peek(0)` |
| `Previous()` | Character before the cursor |
| `Advance()` / `Next()` | Consume and return the current character |
| `Eof()` | True when the cursor is at end-of-input |
| `Position()` | Current byte offset |
| `Rewind(n)` | Move backward by *n* code points |
| `Mark()` / `Reset(mark)` | Save and restore byte offset |
| `Slice(start, end)` | Half-open byte slice into the buffer |

### Character metadata

Each `Character` exposes:

- Unicode scalar value (`char32_t`)
- Byte offset
- Line and column (from `SourceManager`)
- `source::SourceLocation`
- `FutureUnicodeSupport` (UTF-16/UTF-32 column placeholders)

UTF-8 decoding uses `DecodeUtf8()`. Invalid sequences yield U+FFFD and advance one
byte so scanning can recover.

## Lexer coordinator

`Lexer` is a thin coordinator. It does **not** embed tokenization rules.

Responsibilities:

1. Own a `CharacterStream` over the input buffer
2. Build a `LexerContext` shared with scanners
3. Run the scan loop, dispatching `ScannerRegistry`
4. Recover from unknown input without stopping (when configured)
5. Append a terminating `EOF` token
6. Return `LexerResult` with `TokenStream` and `LexerStatistics`

### LexerContext

Mutable per-pass state:

- Reference to `CharacterStream`
- `LexerState` (Normal, InsideString, InsideComment, …)
- Pending tokens produced by scanners
- Error flag
- References to `SourceManager`, `DiagnosticEmitter`, configuration, statistics

### LexerState

| State | Purpose |
|-------|---------|
| `Normal` | Default scanning |
| `InsideString` | String literal bodies |
| `InsideComment` | Block/line comment bodies |
| `InsideDirective` | Preprocessor-style directives |
| `FutureInterpolation` | String interpolation (reserved) |
| `FutureRawString` | Raw string literals (reserved) |

### Configuration

`LexerOptions` → `LexerConfiguration` controls statistics collection, error recovery,
whitespace/trivia handling, and directive/annotation enablement.

`FutureIncrementalLexing` on `LexerConfiguration` reserves fields for IDE
re-lexing of edited regions.

## Scanner architecture

Scanners implement `IScanner` with a single `Scan(LexerContext&)` method returning
`ScanResult`:

| Result | Meaning |
|--------|---------|
| `TokenProduced` | One or more tokens pushed to context |
| `Consumed` | Input consumed, no token (e.g. whitespace) |
| `NotHandled` | Try the next scanner |
| `Error` | Recoverable lexical error |

### Placeholder scanners

Registered in priority order by `ScannerRegistry`:

1. `WhitespaceScanner`
2. `CommentScanner`
3. `DirectiveScanner`
4. `AnnotationScanner`
5. `StringScanner`
6. `CharacterScanner`
7. `NumberScanner`
8. `IdentifierScanner`
9. `KeywordScanner`
10. `OperatorScanner`
11. `UnknownScanner`

All return `NotHandled` today. Future implementations will live in dedicated
translation units without changing the coordinator.

## Future tokenization flow

```
loop until EOF:
    dispatch scanners in order
    if TokenProduced → flush tokens to output vector
    if Consumed       → continue
    if Error          → emit diagnostic + error token, recover
    if NotHandled     → advance one code point, emit recoverable error token
append EOF token
wrap in TokenStream
```

Keyword and operator classification will use `KeywordTable` and `OperatorTable`.
Lexemes remain `string_view` slices into the `SourceBuffer`.

## Error recovery

The lexer **never stops after the first error** when `continue_after_errors` is
enabled (default). Unrecognized input advances one code point, emits a
`TokenType::Error` token with `TokenFlags::Recoverable`, and continues scanning.

## Statistics

`LexerStatistics` tracks:

- Characters read
- Lines processed
- Scanner invocations
- Tokens produced
- Errors encountered
- Elapsed wall time (`std::chrono::steady_clock`)
- Memory usage placeholder (`GetMemoryUsageBytes`)

## Performance considerations

- **Zero-copy lexemes:** characters and future tokens slice `SourceBuffer` text
- **Line table reuse:** line/column lookups use precomputed `LineTable` data
- **Minimal allocation:** `CharacterStream` holds only a cursor; token vector
  reserved at scan start
- **Cache-friendly tables:** keyword/operator lookup remains sorted-array binary
  search (read-only)
- **Streaming:** `CharacterReader` is a non-owning view; large files are not copied
- **UTF-8:** code points decoded on demand; ASCII fast path is a single byte

## Building and testing

```bash
cmake --build build --target myc_lexer
cmake --build build --target myc_tests
ctest --test-dir build -R myc_unit_tests --output-on-failure
```

Unit tests:

- `tests/unit/test_character_stream.cpp` — stream navigation, UTF-8, slices
- `tests/unit/test_lexer_infrastructure.cpp` — lexer init, statistics, recovery
- `tests/unit/test_lexer.cpp` — token system (existing)

## Status

| Component | Status |
|-----------|--------|
| Token system | Implemented |
| CharacterStream | Implemented |
| Lexer framework | Implemented |
| Scanner placeholders | Architecture only |
| Tokenization rules | Pending |
