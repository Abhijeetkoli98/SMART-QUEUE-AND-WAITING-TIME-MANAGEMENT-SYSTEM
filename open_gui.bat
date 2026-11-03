@echo off
tasklist /fi "imagename eq smart_queue.exe" | find /i "smart_queue.exe" >nul
if %ERRORLEVEL% neq 0 (
    echo Starting C++ Native Backend Server on port 3000...
    start "Smart Queue C++ Core Server" smart_queue.exe 3000 --daemon
    timeout /t 2 >nul
)
echo Opening Smart Queue Dashboard at http://localhost:3000 ...
start http://localhost:3000/web_gui/index.html
