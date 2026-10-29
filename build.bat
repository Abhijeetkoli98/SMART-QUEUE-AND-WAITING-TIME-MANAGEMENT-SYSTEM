@echo off
echo ===================================================
echo   Compiling Smart Queue Management System (C++)
echo ===================================================

g++ -std=c++11 -Wall -Wextra src\Token.cpp src\Queue.cpp src\QueueManager.cpp src\main.cpp -o smart_queue.exe

if %ERRORLEVEL% equ 0 (
    echo.
    echo [SUCCESS] Compilation successful!
    echo Executable created: smart_queue.exe
    echo Run 'run.bat' or '.\smart_queue.exe' to start.
) else (
    echo.
    echo [ERROR] Compilation failed. Please check MinGW g++ installation.
)
pause

