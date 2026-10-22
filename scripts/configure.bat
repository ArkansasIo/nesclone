@echo off
setlocal
cd /d "%~dp0.."

where cmake >nul 2>&1
if errorlevel 1 (
  echo [ERROR] CMake was not found in PATH.
  echo Install CMake 3.25 or newer and reopen the terminal.
  exit /b 1
)

if not exist build mkdir build

echo [NEX8] Configuring Visual Studio 2022 x64 build...
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DNEX8_BUILD_ENGINE=ON -DNEX8_BUILD_CELL8=ON -DNEX8_BUILD_RAMDB=ON -DNESCLONE_BUILD_SCRIPTING=ON -DNESCLONE_BUILD_TOOLS=ON -DNESCLONE_BUILD_TESTS=ON
if errorlevel 1 (
  echo [ERROR] CMake configuration failed.
  exit /b 1
)

echo [NEX8] Configuration complete.
exit /b 0
