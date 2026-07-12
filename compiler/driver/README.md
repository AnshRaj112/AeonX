# Compiler Driver

The entry point for the `myc` executable. `CompilerDriver` parses CLI arguments,
loads configuration, initializes diagnostics, and dispatches subcommands.

## Commands

| Command | Description |
|---------|-------------|
| `--help` | Print usage information |
| `--version` | Print version information |
| `build` | Compile `.myc` source files |
| `run` | Build and execute a program |
| `fmt` | Format source code |
| `lint` | Run the linter |
| `benchmark` | Run compiler benchmarks |

## Exit Codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | General error |
| 2 | Invalid arguments |
