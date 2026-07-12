# Tests

Unit and integration tests for the Myc toolchain.

## Running Tests

```bash
cmake --build build --target myc_tests
ctest --test-dir build
```

## Structure

| Directory | Purpose |
|-----------|---------|
| `unit/` | Unit tests per module |
| `integration/` | End-to-end compiler tests (future) |

## Framework

Lightweight in-tree test harness (`myc_test`) with no external dependencies.
