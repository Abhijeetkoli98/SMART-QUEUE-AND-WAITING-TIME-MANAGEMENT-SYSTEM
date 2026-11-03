@echo off
title Smart Queue C++ Terminal Console & HTTP Server [PORT 3000]
echo ===============================================================================
echo   Smart Queue & Waiting-Time Management System (Genuine C++ Core Engine)
echo   INTERACTIVE TERMINAL CONSOLE & LIVE REST SERVER (PORT 3000)
echo ===============================================================================

REM Stop any background daemon if running so port 3000 is free for interactive mode
tasklist /fi "imagename eq smart_queue.exe" | find /i "smart_queue.exe" >nul
if %ERRORLEVEL% equ 0 (
    echo Stopping background server instance to take over terminal...
    taskkill /f /im smart_queue.exe >nul 2>&1
    timeout /t 1 >nul
)

echo Starting interactive C++ Terminal Console...
smart_queue.exe 3000
pause
