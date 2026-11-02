@echo off
title Smart Queue C++ Server [PORT 3000]
echo ===============================================================================
echo   Launching Smart Queue System on Google Chrome (Port 3000 C++ Backend)
echo   PORT: 3000  ^|  KEEP THIS WINDOW OPEN WHILE USING THE SYSTEM
echo ===============================================================================

if not exist smart_queue.exe (
    call build.bat
)

echo Starting C++ Native HTTP Server on port 3000...
start "Smart Queue C++ Core Server" smart_queue.exe 3000 --daemon

timeout /t 2 >nul

echo Opening Google Chrome to http://localhost:3000 ...
if exist "C:\Program Files\Google\Chrome\Application\chrome.exe" (
    start "" "C:\Program Files\Google\Chrome\Application\chrome.exe" "http://localhost:3000/web_gui/index.html"
) else (
    start http://localhost:3000/web_gui/index.html
)

echo.
echo ===============================================================================
echo   Server is running live at: http://localhost:3000/web_gui/index.html
echo   (Press any key to close this launcher)
echo ===============================================================================
pause
