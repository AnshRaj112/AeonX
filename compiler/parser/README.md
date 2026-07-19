# Myc Parser Module

The parser transforms a `TokenStream` from the production lexer into a complete AST. It uses modular recursive-descent parsing with specialized components and error recovery.

## Architecture

```
Parser
  └── ParserContext
        ├── TokenCursor      (peek, consume, match, expect, checkpoint, rewind)
        ├── ParserState      (panic mode, error tracking)
        ├── ParserStatistics (nodes, timing, error counts)
        ├── ParserConfiguration
        └── RecoveryManager  (synchronization points)
              ├── DeclarationParser
              ├── StatementParser
              ├── ExpressionParser
              ├── TypeParser
              ├── LiteralParser
              ├── ImportParser
              ├── AttributeParser
              ├── FuturePatternParser   (placeholder)
              ├── FutureMacroParser     (placeholder)
              └── FutureGenericParser   (placeholder)
```

## Recursive Descent Strategy

Each specialized parser handles one syntactic category:

| Component | Responsibility |
|-----------|----------------|
| `DeclarationParser` | Functions, structs, classes, traits, enums, imports, modules |
| `StatementParser` | Blocks, control flow, variable declarations, returns |
| `ExpressionParser` | Precedence-climbing expressions, calls, member access |
| `TypeParser` | Primitive and named types |
| `LiteralParser` | Integer, float, boolean, string, character literals |
| `ImportParser` | `import path::segments;` |
| `AttributeParser` | `@annotation` prefixes |

The top-level `Parser::ParseProgram()` creates a `ParserContext` and delegates to `DeclarationParser`.

## Token Navigation

`TokenCursor` wraps `TokenStream` with parser-oriented operations:

```cpp
cursor.Peek(0);                    // look at current token
cursor.Consume();                  // advance
cursor.Match(TokenType::KwFn);     // consume if matches
cursor.Expect(TokenType::PunctSemicolon);  // consume or fail
auto cp = cursor.Checkpoint();     // save position
cursor.Rewind(cp);                 // backtrack
```

## Error Recovery

The parser never terminates on the first syntax error when recovery is enabled (default). `RecoveryManager` synchronizes to:

- Semicolons (`;`)
- Closing braces (`}`)
- Declaration-start keywords (`fn`, `struct`, `import`, etc.)
- End of file

Errors are reported through `DiagnosticEmitter` with `MYC0001` (unexpected token) and `MYC0100` (invalid syntax) codes. The parser sets `had_errors` in `ParserResult` but continues parsing when practical.

## Usage

```cpp
myc::lexer::Lexer lexer(buffer, source_manager, emitter);
auto lex_result = lexer.Tokenize();

myc::parser::Parser parser(lex_result.tokens, source_manager, emitter);
myc::parser::ParserResult result = parser.ParseProgram();

if (result.had_errors) { /* handle diagnostics */ }
if (result.program) {
    result.program->Accept(my_visitor);
}
```

## Statistics

`ParserStatistics` collects:

- Nodes created
- Declarations, statements, expressions parsed
- Syntax errors encountered
- Elapsed parsing time

## Future Extensions

Placeholder parsers reserve hooks for:

- **Pattern matching** — `FuturePatternParser` for `match` arms
- **Macros** — `FutureMacroParser` for macro invocation syntax
- **Generics** — `FutureGenericParser` for `Type<T>` and generic declarations

Incremental parsing can reuse `TokenCursor::Checkpoint()` / `Rewind()` and the shared `ParserContext` state model.

## CMake

```cmake
target_link_libraries(your_target PRIVATE myc::parser)
```

## Testing

Parser tests live in `tests/unit/test_parser.cpp` with 250+ cases covering expressions, declarations, error recovery, AST correctness, and large inputs.

Run:

```bash
cmake --build build --target myc_tests
ctest --test-dir build -R myc_unit_tests
```
