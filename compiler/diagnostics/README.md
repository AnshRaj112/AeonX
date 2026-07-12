# Diagnostics Module

Centralized diagnostic reporting for the Myc compiler. All compiler phases emit
diagnostics through `DiagnosticEngine` rather than writing directly to stderr.

## Architecture

```
DiagnosticEngine
    ├── Diagnostic (base)
    │   ├── Error
    │   ├── Warning
    │   ├── Note
    │   └── Hint
    └── FixItHint (future automated fixes)
```

Each diagnostic carries file location, message, error code, and severity.
The engine supports aggregation, filtering, and formatted output.

## Future

- Fix-it suggestions and automated application
- JSON diagnostic output for IDE integration
- Suppression pragmas and warning groups
- rustc-style error codes (e.g., `E0001`)
