#!/bin/bash
# Run the built executable
PROJECT_ROOT="$(dirname "$0")/.."
cd "$PROJECT_ROOT/build"
./hello
