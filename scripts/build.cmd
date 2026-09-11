@echo off
setlocal
cd /d "%~dp0\.."
cmake -B build-win32
cmake --build build-win32 --config Release
