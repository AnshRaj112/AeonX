# Configuration Module

Manages compiler settings, build profiles, target triples, and diagnostic
behavior. Designed for extension without breaking existing callers.

## Key Types

- `CompilerConfig` — top-level configuration aggregate
- `BuildProfile` — Debug / Release / Custom
- `OptimizationLevel` — O0 through O3, Os, Oz
- `TargetArchitecture` — x86_64, aarch64, wasm32, etc.
- `TargetPlatform` — OS and ABI selection
- `DiagnosticConfiguration` — warning levels and error limits
- `GpuConfiguration` — placeholder for GPU backend settings
- `AiConfiguration` — placeholder for AI-assisted compilation settings

## Usage

Configuration is loaded by `CompilerDriver` before any compilation phase runs.
Future support for `myc.toml` project files will extend this module.
