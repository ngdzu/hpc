#!/bin/bash
# Clean build artifacts
PROJECT_ROOT="$(dirname "$0")/.."
BUILD_DIR="$PROJECT_ROOT/build"
if [ -d "$BUILD_DIR" ]; then
    rm -rf "$BUILD_DIR"
    echo "Build directory removed."
else
    echo "No build directory to clean."
fi
