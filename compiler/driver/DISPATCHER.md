# Command Dispatcher

`CommandDispatcher` routes parsed CLI options to the appropriate command implementation
via the `CommandRegistry`. It contains no command-specific logic.

## Dispatch Rules

1. Global `--help` routes to `HelpCommand`
2. Global `--version` routes to `VersionCommand`
3. Missing subcommand emits diagnostic `D0002` and returns exit code 2
4. Unknown subcommand emits diagnostic `D0003` and returns exit code 2
5. Known subcommand: build `CommandContext`, call `AbstractCommand::Execute()`

## CommandContext

Every command receives a `CommandContext` with references to:

- `CompilerConfig` — loaded build/target/diagnostic settings
- `DiagnosticEngine` — for structured error reporting
- `SourceManager` — for loading `.myc` source files
- `CliOptions` — parsed flags, options, and positional arguments

## Error Propagation

If `DiagnosticEngine::HasErrors()` is true after command execution, the dispatcher
overrides the command exit code with `GeneralError` (1).
