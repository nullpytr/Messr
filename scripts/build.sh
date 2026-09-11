#!/bin/sh
set -e
cd "$(dirname "$0")/.."

TARGET=${1:-win32}

case "$TARGET" in
    win32)
        cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/x86_64-w64-mingw32.cmake
        ;;
    native)
        cmake -B build
        ;;
    *)
        echo "Usage: $0 [win32|native]"
        exit 1
        ;;
esac

cmake --build build
