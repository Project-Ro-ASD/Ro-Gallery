#!/usr/bin/env bash
# Install Ro-Gallery for the current user.
set -euo pipefail
cd "$(dirname "$0")"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel "${CMAKE_BUILD_PARALLEL_LEVEL:-2}"
cmake --install build --prefix "$HOME/.local"
update-desktop-database "$HOME/.local/share/applications" >/dev/null 2>&1 || true
echo "Ro-Gallery kuruldu: $HOME/.local/bin/ro-gallery"
