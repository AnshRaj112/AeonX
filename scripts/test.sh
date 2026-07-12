#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${1:-build}"

"${CMAKE_SOURCE_DIR:-.}/scripts/build.sh" Debug "${BUILD_DIR}"
ctest --test-dir "${BUILD_DIR}" --output-on-failure
