#!/bin/bash
# Build the project using Conan and CMake
set -e
PROJECT_ROOT="$(dirname "$0")/.."
cd "$PROJECT_ROOT"
conan install . --output-folder=build --build=missing
# Use the preset defined in CMakePresets.json
cmake --preset=conan-release
cmake --build build
