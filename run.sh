#!/usr/bin/env bash
# Build and launch Ro-Gallery.
set -euo pipefail
cd "$(dirname "$0")"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel "${CMAKE_BUILD_PARALLEL_LEVEL:-2}"
exec ./build/ro-gallery "$@"
