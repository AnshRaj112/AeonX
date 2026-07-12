# Common Utilities

Shared utilities used across the compiler driver modules.

## Result

`myc::common::Result<T>` is a type alias for `std::expected<T, std::string>`.
Used by the CLI parser and other components for recoverable error handling without
exceptions.

## CLI Output

`cli_output.hpp` provides consistent terminal formatting:

- `PrintBanner()` — compiler status banner with version, target, profile
- `PrintPlaceholder()` — standardized placeholder message for unimplemented commands
- `PrintCommandHelp()` — per-command help text
