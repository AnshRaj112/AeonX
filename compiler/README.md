# Compiler

The Myc compiler transforms `.myc` source files through a multi-phase pipeline
into native object code, LLVM IR, or interpreted bytecode.

## Pipeline Phases

```
source → lexer → parser → ast → semantic → typechecker
       → hir → mir → ir → optimizer → codegen → llvm/backend
```

## Submodules

| Module | Responsibility |
|--------|----------------|
| `lexer` | Tokenization of `.myc` source |
| `parser` | Syntax analysis and AST construction |
| `ast` | Abstract syntax tree definitions |
| `semantic` | Name resolution and semantic validation |
| `typechecker` | Type inference and checking |
| `hir` | High-level intermediate representation |
| `mir` | Mid-level IR with explicit control flow |
| `ir` | Low-level IR before code generation |
| `codegen` | Target-independent code generation |
| `llvm` | LLVM backend integration |
| `diagnostics` | Error, warning, and note reporting |
| `driver` | CLI parsing and command dispatch |
| `config` | Compiler and build configuration |
| `source` | Source file and buffer management |

## Design Principles

- Each phase is a separate static library with explicit dependencies.
- No global mutable state; state flows through context objects.
- Diagnostics are collected centrally via `DiagnosticEngine`.
- Configuration is immutable after loading.

## Future Compatibility

Architecture supports incremental compilation, JIT, interpreter mode,
cross-compilation, GPU backends, and WebAssembly emission.
