@echo off
echo ===============================================================================
echo   Launching Smart Queue System on Google Chrome (Port 3000 C++ Backend)
echo ===============================================================================
if not exist smart_queue.exe (
    call build.bat
)

echo Starting C++ Native HTTP Server on port 3000 in background...
start "" "smart_queue.exe" 3000 --daemon

timeout /t 2 >nul

echo Opening Google Chrome to http://localhost:3000 ...
if exist "C:\Program Files\Google\Chrome\Application\chrome.exe" (
    start "" "C:\Program Files\Google\Chrome\Application\chrome.exe" "http://localhost:3000/web_gui/index.html"
) else (
    start http://localhost:3000/web_gui/index.html
)
