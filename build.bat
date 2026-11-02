@echo off
echo ===============================================================================
echo   Compiling Smart Queue & Waiting-Time Management System (Genuine C++ Core)
echo ===============================================================================

g++ -std=c++11 -Wall -Wextra src\Token.cpp src\ServiceQueue.cpp src\Counter.cpp src\QueueEngine.cpp src\HttpServer.cpp src\main.cpp -o smart_queue.exe -lws2_32

if %ERRORLEVEL% equ 0 (
    echo.
    echo [SUCCESS] Compilation successful!
    echo Standalone executable created: smart_queue.exe
    echo Run 'run.bat' or '.\smart_queue.exe' to start.
) else (
    echo.
    echo [ERROR] Compilation failed. Please check MinGW g++ installation.
)
pause
