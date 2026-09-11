#!/bin/sh
set -e
cd "$(dirname "$0")/.."

TARGET=${1:-win32}

case "$TARGET" in
    win32)
        cmake -B build-win32 -DCMAKE_TOOLCHAIN_FILE=cmake/x86_64-w64-mingw32.cmake -DCMAKE_BUILD_TYPE=Release
        cmake --build build-win32
        ;;
    native)
        cmake -B build-native -DCMAKE_BUILD_TYPE=Release
        cmake --build build-native
        ;;
    *)
        echo "Usage: $0 [win32|native]"
        exit 1
        ;;
esac
