@echo off
REM Windows Build Script for Hybrid IDE

echo Building Hybrid IDE for Windows...
echo.

REM Check for CMake
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake not found. Please install CMake.
    exit /b 1
)

REM Create build directory
if not exist build mkdir build
cd build

REM Run CMake
echo Running CMake...
cmake .. -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed.
    cd ..
    exit /b 1
)

REM Build project
echo Building project...
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed.
    cd ..
    exit /b 1
)

cd ..

echo.
echo Build complete!
echo Executables are in build\bin\Release\
echo.
pause
