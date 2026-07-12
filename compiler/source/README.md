# Source Management Module

Provides `SourceManager` for reading, caching, and locating positions within
`.myc` source files.

## Responsibilities

- Load source files into memory buffers
- Track file identifiers and paths
- Map byte offsets to line and column numbers
- Validate UTF-8 encoding
- Future: `#include` / `import` resolution

## Design

Source buffers are immutable once loaded. Line-start offsets are precomputed
for O(log n) line lookup. The manager is owned by the compilation context and
passed by reference to lexer and parser phases.
