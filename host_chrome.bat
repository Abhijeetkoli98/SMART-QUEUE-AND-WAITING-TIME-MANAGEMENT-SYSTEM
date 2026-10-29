@echo off
echo ==========================================================
echo   Hosting Smart Queue Management System on Localhost
echo ==========================================================

echo Starting Node.js HTTP server on http://localhost:3000 ...
start /B node server.js

timeout /t 2 >nul

echo Launching Google Chrome...
if exist "C:\Program Files\Google\Chrome\Application\chrome.exe" (
    start "" "C:\Program Files\Google\Chrome\Application\chrome.exe" "http://localhost:3000"
) else (
    start http://localhost:3000
)

echo.
echo Server is running live at: http://localhost:3000
echo Press any key to stop the server when finished...
pause >nul
taskkill /F /IM node.exe >nul 2>&1
echo Server stopped.

