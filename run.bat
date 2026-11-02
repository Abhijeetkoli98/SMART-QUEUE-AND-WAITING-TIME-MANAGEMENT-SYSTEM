@echo off
echo ===============================================================================
echo   Starting Smart Queue & Waiting-Time Management System (Genuine C++ Core)
echo ===============================================================================
if not exist smart_queue.exe (
    echo smart_queue.exe not found! Building first...
    call build.bat
)

echo Starting C++ Native HTTP REST API Server on port 3000...
start "" "smart_queue.exe" 3000 --daemon

timeout /t 2 >nul

echo Opening Smart Queue Dashboard at http://localhost:3000/web_gui/index.html ...
start http://localhost:3000/web_gui/index.html
