# Command Framework

Extensible command registration and execution framework for the Myc toolchain.

## Architecture

```
AbstractCommand
  ├─ HelpCommand
  ├─ VersionCommand
  ├─ BuildCommand, RunCommand, FormatCommand, LintCommand, BenchmarkCommand
  └─ PlaceholderCommand (test, docs, new, clean, doctor, package, init, repl, ast, tokens, ir, cfg)
```

## Command Registry

`CommandRegistry` is a singleton factory that maps command names to `AbstractCommand`
instances. All built-in commands are registered in `RegisterBuiltInCommands()`.

### Adding a New Command

1. Create `include/myc/commands/my_command.hpp` implementing `AbstractCommand`
2. Create `src/my_command.cpp` with `Execute()` implementation
3. Add the source file to `compiler/commands/CMakeLists.txt`
4. Register in `command_registry.cpp`:

```cpp
Register(std::make_unique<MyCommand>());
```

No changes to `CompilerDriver` or `CommandDispatcher` are required.

## AbstractCommand Interface

| Method | Purpose |
|--------|---------|
| `GetName()` | Command name used on the CLI |
| `GetDescription()` | Short description for help output |
| `GetSynopsis()` | Usage synopsis |
| `Execute(ctx)` | Run the command, return `ExitCode` |

## Placeholder Commands

`PlaceholderCommand` provides consistent output for features not yet implemented.
Each placeholder prints the status banner and a message describing the planned phase.
