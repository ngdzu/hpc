#!/bin/bash
# Run the built executable
PROJECT_ROOT="$(dirname "$0")/.."
PROJECT_NAME=$(grep '^PROJECT_NAME=' "$PROJECT_ROOT/project_config.txt" | cut -d'=' -f2)
cd "$PROJECT_ROOT/build"
./"$PROJECT_NAME"
