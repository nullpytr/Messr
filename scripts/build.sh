#!/bin/sh
set -e
cd "$(dirname "$0")/.."

TARGET=${1:-win64}

case "$TARGET" in
    win64)
        cmake -B build/win64 -DCMAKE_TOOLCHAIN_FILE=cmake/x86_64-w64-mingw32.cmake -DCMAKE_BUILD_TYPE=Release
        cmake --build build/win64
        ;;
    native)
        cmake -B build/native -DCMAKE_BUILD_TYPE=Release
        cmake --build build/native
        ;;
    *)
        echo "Usage: $0 [win64|native]"
        exit 1
        ;;
esac
