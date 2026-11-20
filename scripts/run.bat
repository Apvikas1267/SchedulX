@echo off
REM Starts the C backend (which also serves the frontend) and opens the browser.
setlocal
cd /d "%~dp0\.."
set PORT=8080
if not exist backend\bin\schedulx.exe (
  echo Backend not built yet - running setup first ...
  call scripts\setup.bat
  if errorlevel 1 exit /b 1
)
echo Starting SchedulX on http://localhost:%PORT% ^(press Ctrl+C to stop^)
start "" cmd /c "timeout /t 2 /nobreak >nul & start http://localhost:%PORT%"
backend\bin\schedulx.exe %PORT% frontend
