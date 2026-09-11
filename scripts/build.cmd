@echo off
setlocal
cd /d "%~dp0\.."
cmake -B build/win64
cmake --build build/win64 --config Release
