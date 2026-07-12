# Compiler Driver

The `myc` executable entry point. `CompilerDriver` owns configuration, diagnostics,
source management, and delegates all command execution to `CommandDispatcher`.

## Responsibilities

- Parse CLI arguments via `myc::cli::CliParser`
- Configure logging levels from global flags (`--verbose`, `--quiet`, `--trace`)
- Load `CompilerConfig` from CLI options and environment variables
- Initialize the diagnostic engine
- Dispatch commands through the registry — never implement command logic directly
- Catch all exceptions and convert them to diagnostics

## CLI Flow

```
main()
  └─ CompilerDriver::Run(argc, argv)
       ├─ CliParser::Parse()           → Result<CliOptions>
       ├─ ConfigureLogging()
       ├─ LoadConfiguration()          → CompilerConfig from CLI
       ├─ LoadEnvironmentOverrides()   → MYC_PROFILE, MYC_TARGET, ...
       ├─ InitializeDiagnostics()
       └─ CommandDispatcher::Dispatch()
            ├─ --help    → HelpCommand
            ├─ --version → VersionCommand
            └─ <command> → CommandRegistry::Find() → AbstractCommand::Execute()
```

## Environment Variables

| Variable | Effect |
|----------|--------|
| `MYC_PROFILE` | Override build profile (`debug`, `release`, ...) |
| `MYC_TARGET` | Override target architecture (`x86_64`, `aarch64`, ...) |
| `MYC_PLATFORM` | Override target platform (`linux`, `windows`, ...) |
| `MYC_VERBOSE` | Enable debug logging when set to `1` or `true` |

## Exit Codes

See `compiler/exitcodes/` for the centralized `ExitCode` enumeration.

## Adding Future Commands

Do **not** modify `CompilerDriver`. Register new commands in
`compiler/commands/src/command_registry.cpp`. See `compiler/commands/README.md`.
