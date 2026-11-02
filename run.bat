@echo off
echo ===============================================================================
echo   Starting Smart Queue & Waiting-Time Management System (Genuine C++ Core)
echo ===============================================================================
if not exist smart_queue.exe (
    echo smart_queue.exe not found! Building first...
    call build.bat
)

echo Starting C++ Native HTTP REST API Server on port 8080...
start "" "smart_queue.exe" 8080

timeout /t 2 >nul

echo Opening Smart Queue Dashboard in default browser...
start http://localhost:8080/web_gui/index.html
