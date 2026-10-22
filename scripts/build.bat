@echo off
setlocal
cd /d "%~dp0.."

if not exist build (
  call scripts\configure.bat
  if errorlevel 1 exit /b 1
)

echo [NEX8] Building Release...
cmake --build build --config Release --parallel
if errorlevel 1 (
  echo [ERROR] Build failed.
  exit /b 1
)

echo [NEX8] Release build complete.
exit /b 0
