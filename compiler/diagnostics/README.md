# Diagnostics Module

Centralized diagnostic reporting for the Myc compiler. Every compiler phase emits
structured diagnostics through `DiagnosticEngine` rather than writing directly to
`stderr`.

## Architecture

```
Compiler Phase
      │
      ▼
DiagnosticEmitter / DiagnosticBuilder
      │
      ▼
DiagnosticEngine  ──collects──▶  vector<Diagnostic>
      │                                   │
      │                                   ▼
      └────────────────────────▶  DiagnosticReporter
                                          │
                          ┌───────────────┼───────────────┐
                          ▼               ▼               ▼
                 DiagnosticFormatter  DiagnosticJsonWriter  DiagnosticLspSerializer
                   (plain text)           (JSON)              (future LSP)
```

### Core Types

| Type | Responsibility |
|------|----------------|
| `Severity` | `Fatal`, `Error`, `Warning`, `Note`, `Hint`, `Info` |
| `ErrorCode` | Unique codes such as `MYC0001`, centrally registered in `codes::` |
| `SourceLocation` | Filename, absolute path, line, column |
| `SourceSpan` | Multi-line byte range with offsets and source length |
| `Diagnostic` | Primary message with attached notes, hints, and fix-its |
| `DiagnosticBuilder` | Fluent construction API |
| `DiagnosticEngine` | Thread-safe collection, limits, and policy enforcement |
| `DiagnosticEmitter` | Phase-facing convenience wrapper around the engine |
| `DiagnosticFormatter` | Human-readable rustc/clang-style rendering |
| `DiagnosticReporter` | Routes diagnostics to the selected output format |
| `DiagnosticJsonWriter` | Machine-readable JSON serialization |

## Design Rationale

- **Separation of collection and rendering** — compiler phases only emit diagnostics;
  the reporter decides how they are displayed.
- **No globals** — each driver owns a `DiagnosticEngine` instance passed through
  `CommandContext`.
- **Policy-driven behavior** — warning limits, suppression, warnings-as-errors, and
  error recovery are configured through `DiagnosticsPolicy` (mapped from
  `config::DiagnosticConfiguration`).
- **Thread-safe collection** — `DiagnosticEngine` guards its storage with a mutex to
  support future parallel compilation.
- **Extensible output** — plain text and JSON are implemented; LSP output is reserved
  behind `DiagnosticLspSerializer` for language-server integration.

## Usage Examples

### Simple emission

```cpp
myc::diagnostics::DiagnosticEngine engine;
engine.EmitError("MYC0001", "unexpected token");
```

### Fluent builder

```cpp
myc::diagnostics::DiagnosticBuilder(engine, myc::diagnostics::Severity::Error)
    .Error(myc::diagnostics::codes::UnexpectedToken, "unexpected token")
    .At(span)
    .Label("expected expression")
    .Note("expressions cannot be empty")
    .Hint("insert a valid expression before ')'")
    .Emit();
```

### Reporting with source context

```cpp
myc::diagnostics::DiagnosticReporter::Options options;
options.format = myc::diagnostics::DiagnosticOutputFormat::PlainText;
options.colors_enabled = true;
options.source_manager = &source_manager;

myc::diagnostics::DiagnosticReporter reporter(options);
reporter.Report(engine, std::cerr);
```

### JSON output

```cpp
options.format = myc::diagnostics::DiagnosticOutputFormat::Json;
reporter.Report(engine, std::cout);
```

### Phase integration

```cpp
void Analyze(myc::diagnostics::DiagnosticEmitter& emitter) {
    emitter.Builder(myc::diagnostics::Severity::Warning)
        .Warning(myc::diagnostics::codes::TypeMismatch, "expected int, found string")
        .Emit();
}
```

## Error Codes

Codes are defined in `error_code.hpp` under `myc::diagnostics::codes::`. Extend the
registry as new compiler phases are implemented:

| Code | Meaning |
|------|---------|
| `MYC0001` | Unexpected token |
| `MYC0002` | Empty expression |
| `MYC0100` | Invalid syntax |
| `MYC1000` | Type mismatch |
| `MYC9001`–`MYC9999` | Driver and infrastructure errors |

Generate new codes with `ErrorCode::FromNumber(n)`.

## Configuration

`DiagnosticsPolicy` controls runtime behavior:

| Field | Effect |
|-------|--------|
| `warnings_as_errors` | Promote warnings to error exit status |
| `suppress_warnings` | Drop warning diagnostics |
| `show_notes` / `show_hints` | Filter secondary messages |
| `error_recovery` | Allow compilation to continue after errors |
| `max_diagnostics` | Cap total collected diagnostics (0 = unlimited) |
| `error_limit` / `warning_limit` | Per-severity caps |

## Future Extensions

- **LSP integration** — implement `DiagnosticLspSerializer` for
  `textDocument/publishDiagnostics`.
- **Localization** — attach message keys to `ErrorCode` and resolve at format time.
- **Suppression pragmas** — `DiagnosticsPolicy` can gain warning-group tables.
- **Fix-it application** — `FixItHint` is already modeled; application belongs in
  the formatter/IDE layer.
- **Parallel compilation** — per-thread diagnostic buffers merged into the engine.

## Dependencies

- `myc_shared` — common type aliases (`FileId`, `LineNumber`, `ColumnNumber`)
- `myc_source` — source snippets in formatted output (private dependency)
