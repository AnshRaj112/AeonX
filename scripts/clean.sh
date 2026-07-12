#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${1:-build}"

if [[ -d "${BUILD_DIR}" ]]; then
    rm -rf "${BUILD_DIR}"
    echo "Removed ${BUILD_DIR}"
else
    echo "Nothing to clean"
fi
