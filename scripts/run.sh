#!/bin/sh
# Linux/macOS convenience script (the project targets Windows, but this also works with gcc + make).
cd "$(dirname "$0")/../backend" && make && ./bin/schedulx 8080 ../frontend
