@echo off

REM Check if the build directory exists; if not, create it
if not exist build (
    mkdir build
    cd build 
    cmake -G "Ninja" ^
    -D CMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang++.exe" ^
    -D CMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang.exe" ^
    -D CMAKE_C_COMPILER_LAUNCHER=ccache ^
    -D CMAKE_CXX_COMPILER_LAUNCHER=ccache ^
    -D CMAKE_COLOR_DIAGNOSTICS=ON ^
    -D CMAKE_CXX_FLAGS:STRING="-fdiagnostics-color=always" ^
    -D CMAKE_C_FLAGS:STRING="-fdiagnostics-color=always" ^
    ..
    cd ..
)

REM Run CMake with 6 threads
cmake --build build --target MyRaylibGame -j6  

if %errorlevel% neq 0 (
    echo Errors detected in build. Exiting.
    exit /b 1
)

if "%1"=="run" call Run.bat
if "%2"=="run" call Run.bat
