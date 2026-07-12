# CLI Parser

Reusable argument parser for the Myc toolchain. Designed for extensibility without
external dependencies.

## Features

- Global flags parsed at any position
- Subcommand detection
- Source file collection (`.myc` extension)
- Key-value options (`--key=value`)
- Future-ready flags: `--json`, `--time`, `--diagnostics`, `-O<level>`
- Returns `Result<CliOptions>` for recoverable parse errors

## Global Flags

| Flag | Field |
|------|-------|
| `--help`, `-h` | `show_help` |
| `--version`, `-V` | `show_version` |
| `--verbose`, `-v` | `verbose` |
| `--quiet`, `-q` | `quiet` |
| `--color` / `--no-color` | `color_mode` |
| `--json` | `json_output` |
| `--profile <name>` | `profile` |
| `--target <triple>` | `target` |
| `--platform <name>` | `platform` |
| `-O<level>` | `optimization` |
| `--time` | `time_report` |
| `--trace` | `trace` |
| `--diagnostics` | `diagnostics_output` |

## Extending the Parser

Add new flags in `cli_parser.cpp` inside `ParseGlobalFlag()`. Add corresponding
fields to `CliOptions` in `cli_options.hpp`. Unknown `--long-flags` are collected
in `unknown_flags` for forward compatibility.
