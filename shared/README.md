# Shared Module

Cross-cutting infrastructure shared by the compiler, runtime, and tooling.

## Contents

| Component | Responsibility |
|-----------|----------------|
| `version` | Language name, compiler version, build metadata |
| `logging` | Lightweight, replaceable logging facade |
| `types` | Common type aliases and forward declarations |

## Design

All shared code lives under the `myc` namespace. Modules expose header-only or
small compiled libraries with no global mutable state. Dependencies flow inward:
shared modules must not depend on compiler-specific code.

## Future

- Memory allocator abstractions
- Error handling utilities
- Platform detection
- File system helpers
