@echo off
setlocal
cd /d "%~dp0.."

if not exist build\Release\nesclone.exe (
  call scripts\build.bat
  if errorlevel 1 exit /b 1
)

build\Release\nesclone.exe %*
exit /b %errorlevel%
