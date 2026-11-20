@echo off
REM One-time setup: checks the compiler, builds the backend and runs the self-tests.
setlocal
cd /d "%~dp0\.."
echo === SchedulX setup ===
where gcc >nul 2>nul
if errorlevel 1 (
  echo [ERROR] gcc not found. Install MinGW-w64 ^(MSYS2: pacman -S mingw-w64-ucrt-x86_64-gcc^) and add its bin folder to PATH.
  exit /b 1
)
gcc --version | findstr /i gcc
call backend\build.bat
if errorlevel 1 exit /b 1
echo.
echo Running backend tests ...
call backend\build.bat test
if errorlevel 1 ( echo [ERROR] Tests failed. & exit /b 1 )
echo.
echo Setup finished. Start the app with scripts\run.bat
