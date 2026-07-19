# Myc Programming Language

A next-generation systems programming language and toolchain, designed for
long-term scalability from language design through compilation, runtime, and IDE
integration.

## Repository Layout

```
AeonX/
├── compiler/          # Compiler pipeline (lexer → codegen → llvm)
├── runtime/           # Concurrency runtime and memory management
├── stdlib/            # Standard library
├── dsa/               # Shared data structures and algorithms
├── optimizer/         # Optimization passes
├── backend/           # Target backends (native, WASM, GPU)
├── frontend/          # Frontend tooling wrapper
├── package-manager/   # Dependency and package management
├── language-server/   # LSP implementation
├── formatter/         # Source formatter
├── linter/            # Static analysis
├── debugger/          # Debug info and protocol support
├── ide/               # IDE integration
├── shared/            # Cross-cutting infrastructure
├── tests/             # Unit and integration tests
├── benchmarks/        # Performance benchmarks
├── examples/          # Sample .myc programs
├── scripts/           # Build and CI scripts
├── third_party/       # Vendored dependencies (minimal)
└── docs/              # Documentation
```

## Requirements

- CMake 3.25 or later
- C++23-capable compiler (GCC 13+, Clang 16+, or MSVC 19.34+)
- Ninja (recommended) or Make

## Build Instructions

### Quick Start

```bash
# Configure (Debug)
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build
cmake --build build --parallel

# Run tests
ctest --test-dir build --output-on-failure
```

### Release Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

### Using Scripts

```bash
bash scripts/build.sh Release build
bash scripts/test.sh build
```

The compiler executable is produced at `build/bin/myc` (or `build\Debug\bin\myc` on
multi-config generators).

## Compiler Usage

```bash
myc --help
myc --version
myc build main.myc
myc run examples/hello.myc
myc fmt src/
myc lint src/
myc benchmark
```

Source files use the `.myc` extension.

## Architecture

See [docs/architecture.md](docs/architecture.md) for the full system design.

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `MYC_BUILD_TESTS` | ON | Build unit tests |
| `MYC_BUILD_BENCHMARKS` | OFF | Build benchmarks |
| `MYC_ENABLE_SANITIZERS` | OFF | Enable ASan/UBSan (Debug) |
| `MYC_ENABLE_LTO` | OFF | Link-time optimization (Release) |

## Design Principles

- **C++23** with RAII, smart pointers, and no global mutable state
- **Strict modularity** — each phase is an independent static library
- **LLVM-compatible** pipeline architecture for future backend integration
- **Warnings as errors** in all builds
- **Extensible configuration** for targets, GPU, and AI-assisted compilation

## License

License to be determined.


<!--
This language is going to be my personal language that I have developed
-!>
