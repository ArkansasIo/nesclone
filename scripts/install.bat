@echo off
setlocal
cd /d "%~dp0.."

echo ========================================
echo NEX8 Windows Install / Build
echo ========================================

echo [1/3] Configuring...
call scripts\configure.bat
if errorlevel 1 exit /b 1

echo [2/3] Compiling...
call scripts\build.bat
if errorlevel 1 exit /b 1

echo [3/3] Testing...
call scripts\test.bat
if errorlevel 1 exit /b 1

echo.
echo NEX8 build installed into:
echo   %cd%\build\Release
echo.
echo Main executable:
echo   %cd%\build\Release\nesclone.exe
exit /b 0
