@echo off
setlocal
cd /d "%~dp0.."

if not exist build\Release\nex8_ramdb_tests.exe (
  call scripts\build.bat
  if errorlevel 1 exit /b 1
)

build\Release\nex8_ramdb_tests.exe
exit /b %errorlevel%
