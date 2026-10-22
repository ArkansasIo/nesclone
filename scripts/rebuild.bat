@echo off
setlocal
cd /d "%~dp0.."

call scripts\clean.bat
if errorlevel 1 exit /b 1

call scripts\configure.bat
if errorlevel 1 exit /b 1

call scripts\build.bat
if errorlevel 1 exit /b 1

call scripts\test.bat
if errorlevel 1 exit /b 1

echo [NEX8] Rebuild and test completed.
exit /b 0
