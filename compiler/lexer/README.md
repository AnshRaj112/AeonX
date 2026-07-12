# Lexer Module — Token System

The lexer module provides the lexical token infrastructure for the Myc compiler.
This layer defines the immutable data structures that the lexer will populate and
that every downstream phase (parser, semantic analysis, formatter, language server)
will consume.

Lexical analysis itself is **not** implemented here — only the token architecture.

## Architecture

```
SourceBuffer
     │
     ▼
Lexer (future) ──► TokenFactory ──► Token
                         │              │
                         │         SourceLocation / SourceSpan
                         ▼
                   TokenStream ──► TokenIterator
                         │
                         ▼
                   Parser (future)
```

### Supporting tables

```
KeywordTable ──► Keyword { text, TokenType, reserved, version, feature_gate }
OperatorTable ──► Operator { symbol, precedence, associativity, arity }
```

## Core types

| Type | Responsibility |
|------|----------------|
| `Token` | Immutable lexical unit with type, lexeme, location, span, flags, and category |
| `TokenType` | Comprehensive `enum class` covering identifiers, keywords, literals, operators, punctuation, delimiters, directives, and trivia |
| `TokenCategory` | Higher-level grouping to simplify parser dispatch |
| `TokenFlags` | Bit flags for trivia, macro expansion, invalid tokens, and lexer metadata |
| `LiteralKind` | Discriminates integer, float, string, and future interpolated literal forms |
| `Keyword` / `KeywordTable` | Centralized keyword database with binary-search lookup |
| `Operator` / `OperatorTable` | Operator precedence, associativity, and arity metadata |
| `TokenFactory` | Sole construction path for immutable `Token` values |
| `TokenStream` | Navigable, shareable token sequence with peek/consume/checkpoint support |
| `TokenIterator` | STL random-access iterator for range-based loops |
| `TokenPrinter` | Human-readable debug output (`Identifier("count")`, `EOF`, etc.) |
| `FutureTrivia` | Placeholder for leading/trailing trivia attachment |
| `FutureMacroToken` | Placeholder for macro expansion provenance |
| `FutureSemanticValue` | Placeholder for parsed literal semantic values |

## Design decisions

**Immutable tokens.** Tokens are constructed exclusively through `TokenFactory` and
cannot be modified afterward. Lexemes are stored as `std::string_view` slices into
the source buffer, avoiding per-token heap allocations.

**Grouped `TokenType` values.** Enumerators are allocated in reserved numeric ranges
per category (keywords 100–199, literals 300–319, operators 400–549, etc.) so new
language constructs can be added without renumbering existing values.

**Separated navigation from storage.** `TokenStream` stores tokens in a
`shared_ptr`-backed immutable vector. Multiple streams can share the same storage
via `Share()` while maintaining independent cursor positions — enabling future
parallel compiler stages to read tokens concurrently.

**Centralized lookup tables.** Keywords and operators live in sorted `constexpr`
arrays with `std::lower_bound` lookup. This avoids hash maps, keeps tables in
read-only memory, and supports millions of lookups with predictable performance.

**Category derivation.** `GetCategory(TokenType)` maps every `TokenType` to a
`TokenCategory` at construction time and caches the result on the token, so the
parser can branch on coarse categories without repeated range checks.

## Relationships

### Token ↔ Source Manager

Every token carries a `source::SourceLocation` (start position) and
`source::SourceSpan` (half-open byte range). The lexeme `string_view` points into
the owning `SourceBuffer` text, so tokens remain valid as long as the buffer lives.

### Token ↔ Lexer (future)

The lexer will:
1. Scan characters from `SourceBuffer`
2. Classify lexemes using `KeywordTable` and `OperatorTable`
3. Construct tokens via `TokenFactory`
4. Append them to a `std::vector<Token>` and wrap in `TokenStream`

### Token ↔ Parser (future)

The parser will:
1. Receive a `TokenStream` from the lexer
2. Use `Peek()` / `Consume()` / `SaveCheckpoint()` for speculative parsing
3. Branch on `TokenCategory` and `TokenType`
4. Consult `OperatorTable` for expression precedence parsing

### Token ↔ Diagnostics

Lexer errors will create `TokenType::Error` tokens with `TokenFlags::Invalid`.
Diagnostic emission converts `source::SourceSpan` to `diagnostics::SourceSpan`
via the source manager (bridge to be implemented with the lexer).

## Future extensions

| Extension | Hook |
|-----------|------|
| Trivia attachment | `FutureTrivia` on `Token`, `TokenFlags::HasLeadingTrivia` |
| Macro expansion | `FutureMacroToken`, `TokenFlags::FromMacroExpansion` |
| Parsed literal values | `FutureSemanticValue` |
| Interpolated strings | `LiteralKind::InterpolatedString` |
| User-defined operators | `Operator::overload_set_id` |
| Feature-gated keywords | `Keyword::feature_gate`, `Keyword::language_version` |
| Incremental re-lexing | `TokenStream::Share()` + immutable token vectors |
| Parser checkpoints | `SaveCheckpoint()` / `RestoreCheckpoint()` |

## Building and testing

```bash
cmake --build build --target myc_lexer
cmake --build build --target myc_tests
ctest --test-dir build -R myc_unit_tests --output-on-failure
```

Unit tests live in `tests/unit/test_lexer.cpp`.

## Status

Token infrastructure: **implemented**

Lexer (character scanning): **pending**
