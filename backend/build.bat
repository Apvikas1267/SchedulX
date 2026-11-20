@echo off
REM Builds the SchedulX backend with GCC (MinGW-w64). Usage: build.bat [test]
setlocal
cd /d "%~dp0"
where gcc >nul 2>nul
if errorlevel 1 (
  echo [ERROR] gcc was not found on PATH. Install MinGW-w64 ^(e.g. MSYS2 or winlibs^) and reopen the terminal.
  exit /b 1
)
if not exist bin mkdir bin
set CORE=src\strbuf.c src\linkedlist.c src\hashtable.c src\queue.c src\stack.c src\heap.c src\graph.c src\process.c src\stats.c src\scheduler.c
set FLAGS=-std=gnu11 -Wall -Wextra -O2 -Iinclude -D__USE_MINGW_ANSI_STDIO=1

if /i "%1"=="test" goto :test

echo Compiling schedulx.exe ...
gcc %FLAGS% %CORE% src\api.c src\http_server.c src\main.c -o bin\schedulx.exe -lws2_32
if errorlevel 1 ( echo [ERROR] Build failed. & exit /b 1 )
echo Build OK: backend\bin\schedulx.exe
exit /b 0

:test
echo Compiling tests ...
gcc %FLAGS% %CORE% tests\test_scheduler.c -o bin\test_scheduler.exe -lm
if errorlevel 1 ( echo [ERROR] Test build failed. & exit /b 1 )
bin\test_scheduler.exe
exit /b %errorlevel%
