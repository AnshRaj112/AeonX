# Exit Codes

Centralized process exit codes for the entire Myc toolchain.

| Code | Enum | Meaning |
|------|------|---------|
| 0 | `Success` | Operation completed successfully |
| 1 | `GeneralError` | General failure (diagnostics emitted) |
| 2 | `InvalidArguments` | Invalid or missing CLI arguments |
| 3 | `FileNotFound` | Required file does not exist |
| 4 | `ConfigurationError` | Invalid configuration |
| 5 | `InternalCompilerError` | Unhandled internal failure |
| 6 | `Reserved` | Reserved for future use |

All commands return `exitcodes::ExitCode`. The driver converts to `int` via `ToInt()`.
