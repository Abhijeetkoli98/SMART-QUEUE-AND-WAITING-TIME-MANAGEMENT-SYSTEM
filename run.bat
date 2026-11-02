@echo off
title Smart Queue C++ Server [PORT 3000]
echo ===============================================================================
echo   Smart Queue & Waiting-Time Management System (Genuine C++ Core Engine)
echo   PORT: 3000  ^|  KEEP THIS WINDOW OPEN WHILE USING THE SYSTEM
echo ===============================================================================

if not exist smart_queue.exe (
    echo smart_queue.exe not found! Building first...
    call build.bat
)

echo Starting C++ Native HTTP Server on port 3000...
start "Smart Queue C++ Core Server" smart_queue.exe 3000 --daemon

timeout /t 2 >nul

echo.
echo Opening Smart Queue Dashboard in your browser...
start http://localhost:3000/web_gui/index.html
echo.
echo ===============================================================================
echo   Server is running live at: http://localhost:3000/web_gui/index.html
echo   (Press any key to close this launcher)
echo ===============================================================================
pause
