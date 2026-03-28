@echo off

:: Check if the build directory exists; if not, create it
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

REM Run CMake with the appropriate build type
@REM cmake --build build --target MyRaylibGame -j6  
powershell -NoProfile -ExecutionPolicy Bypass -Command "$log='bin\\debug\\Compile.log'; $header='=== Build Output (' + (Get-Date -Format 'yyyy-MM-dd HH:mm:ss') + \") ===`n`n\"; $filtered = @(); cmake --build build --target MyRaylibGame -j6 2>&1 | ForEach-Object { Write-Host $_; if (-not ($_ -match '^ccache ' -or $_ -match '^clang\+\+ ' -or $_ -match '^[A-Z]:\\\\PROGRA~' -or $_ -match '\\.exe ' -or $_.Trim() -eq '')) { $filtered += $_ } }; $body = if ($filtered) { $filtered -join \"`n\" } else { 'Build succeeded - no warnings or errors.' }; \"$header$body`n\" | Set-Content -Path $log -Encoding utf8 -Force; exit $LASTEXITCODE"


if %errorlevel% neq 0 (
    echo Errors detected in build. Exiting.
    exit /b 1
)

if "%1"=="run" call Run.bat
if "%2"=="run" call Run.bat
