@echo off

REM Check if the build directory exists; if not, create it
if not exist build (
    mkdir build
    cd build 
    
    cmake -G "Ninja" ^
    -D CMAKE_CXX_COMPILER=clang++ ^
    -D CMAKE_BUILD_TYPE=Release ^
    ..

    cd ..
)

REM Build the app with cmake
cmake --build build --target MyRaylibGame --config Release

if %errorlevel% neq 0 (
    echo Errors detected in build. Exiting.
    exit /b 1
)

if "%1"=="run" call Run.bat
if "%2"=="run" call Run.bat
