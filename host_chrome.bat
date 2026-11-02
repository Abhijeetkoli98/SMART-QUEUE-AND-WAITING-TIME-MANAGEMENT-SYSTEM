@echo off
echo ===============================================================================
echo   Launching Smart Queue System on Google Chrome (Connected to C++ Backend)
echo ===============================================================================
if not exist smart_queue.exe (
    call build.bat
)

echo Starting C++ Native HTTP Server...
start "" "smart_queue.exe" 8080

timeout /t 2 >nul

echo Opening Google Chrome to C++ Native Server...
if exist "C:\Program Files\Google\Chrome\Application\chrome.exe" (
    start "" "C:\Program Files\Google\Chrome\Application\chrome.exe" "http://localhost:8080/web_gui/index.html"
) else (
    start http://localhost:8080/web_gui/index.html
)
