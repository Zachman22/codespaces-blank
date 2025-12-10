@echo off
REM CMake Build Script for Hybrid IDE (Windows)
REM One-click build with automatic dependency detection

echo ========================================
echo   Hybrid IDE - CMake Build
echo ========================================
echo.

REM Check for CMake
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] CMake not found. Please install CMake 3.15 or later.
    echo   Download from: https://cmake.org/download/
    pause
    exit /b 1
)

for /f "tokens=*" %%i in ('cmake --version') do (
    echo [OK] CMake found: %%i
    goto :cmake_found
)
:cmake_found

REM Check for C++ compiler
where g++ >nul 2>&1
if %errorlevel% equ 0 (
    for /f "tokens=*" %%i in ('g++ --version') do (
        echo [OK] g++ found: %%i
        goto :compiler_found
    )
)

where cl >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] MSVC found
    set GENERATOR="Visual Studio 17 2022"
    goto :compiler_found
)

echo [ERROR] No C++ compiler found. Please install:
echo   MinGW-w64: https://www.mingw-w64.org/
echo   Visual Studio: https://visualstudio.microsoft.com/
pause
exit /b 1

:compiler_found

REM Check for Node.js
where node >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARNING] Node.js not found. Frontend will not be built.
    echo   Install from: https://nodejs.org/
    set BUILD_FRONTEND=0
) else (
    for /f "tokens=*" %%i in ('node --version') do echo [OK] Node.js found: %%i
    set BUILD_FRONTEND=1
)

echo.

REM Build frontend
if %BUILD_FRONTEND% equ 1 (
    echo [1/3] Building React frontend...
    cd frontend
    
    if not exist "node_modules" (
        echo Installing npm dependencies...
        call npm install
        if %errorlevel% neq 0 (
            echo [ERROR] npm install failed
            cd ..
            pause
            exit /b 1
        )
    )
    
    echo Building frontend with Vite...
    call npm run build
    if %errorlevel% neq 0 (
        echo [ERROR] Frontend build failed
        cd ..
        pause
        exit /b 1
    )
    
    cd ..
    echo [OK] Frontend build complete
    echo.
)

REM Create build directory
echo [2/3] Configuring CMake...
if exist "build" rmdir /s /q "build"
mkdir build
cd build

REM Configure with CMake
if defined GENERATOR (
    cmake .. -G %GENERATOR% -DCMAKE_BUILD_TYPE=Release
) else (
    cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
)

if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed
    cd ..
    pause
    exit /b 1
)

echo [OK] CMake configuration complete
echo.

REM Build
echo [3/3] Building C++ backend...
cmake --build . --config Release

if %errorlevel% neq 0 (
    echo [ERROR] Build failed
    cd ..
    pause
    exit /b 1
)

echo [OK] Build complete
echo.

cd ..

REM Summary
echo ========================================
echo   Build Complete!
echo ========================================
echo.
echo Executable: build\bin\HybridIDE.exe
echo.
echo To install:
echo   cmake --install build
echo.
echo To create installer package:
echo   cd build ^&^& cpack
echo.
echo To run:
echo   .\build\bin\HybridIDE.exe
echo.

pause
