@echo off
if not exist smart_queue.exe (
    echo smart_queue.exe not found! Building first...
    call build.bat
)
smart_queue.exe

