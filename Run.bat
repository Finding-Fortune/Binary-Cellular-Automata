@echo off
set EXE_PATH=build\bin\MyRaylibGame.exe

if exist %EXE_PATH% (
    echo Launching MyRaylibGame...
    start "" %EXE_PATH%
) else (
    echo Error: Executable not found. Did you run Compile.bat first?
    pause
)