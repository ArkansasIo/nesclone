@echo off
setlocal
cd /d "%~dp0.."

if not exist build (
  echo [ERROR] Build directory does not exist. Run configure.bat and build.bat first.
  exit /b 1
)

echo [NEX8] Running Release tests...
ctest --test-dir build -C Release --output-on-failure
if errorlevel 1 (
  echo [ERROR] One or more tests failed.
  exit /b 1
)

echo [NEX8] All tests passed.
exit /b 0
