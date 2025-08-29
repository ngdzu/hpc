#!/bin/bash
# Build the project using Conan and CMake
set -e
PROJECT_ROOT="$(dirname "$0")/.."
cd "$PROJECT_ROOT"
conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build .
