@echo off
echo ========================================
echo    Hybrid IDE - Windows Build Script
echo ========================================
echo.

REM Step 1: Build React Frontend
echo [1/3] Building React frontend...
cd frontend
call npm install
if %errorlevel% neq 0 (
    echo ERROR: npm install failed
    pause
    exit /b 1
)

call npm run build
if %errorlevel% neq 0 (
    echo ERROR: Frontend build failed
    pause
    exit /b 1
)

cd ..
echo [SUCCESS] Frontend built successfully!
echo.

REM Step 2: Compile C++ Backend
echo [2/3] Compiling C++ backend...

g++ -std=c++17 ^
    -Iinclude ^
    -DUNICODE -D_UNICODE ^
    src/main.cpp ^
    src/webview_host.cpp ^
    src/compiler.cpp ^
    src/system_info.cpp ^
    src/prompt_verifier.cpp ^
    src/code_signer.cpp ^
    src/file_operations.cpp ^
    -o HybridIDE.exe ^
    -lole32 -loleaut32 -luuid -lshell32 -luser32 -lcrypt32 -lssl -lcrypto ^
    -mwindows ^
    -static-libgcc -static-libstdc++

if %errorlevel% neq 0 (
    echo ERROR: C++ compilation failed
    echo.
    echo Make sure MinGW is installed and in your PATH
    echo Try: g++ --version
    pause
    exit /b 1
)

echo [SUCCESS] C++ backend compiled successfully!
echo.

REM Step 3: Verify Output
echo [3/3] Verifying output...
if exist HybridIDE.exe (
    echo [SUCCESS] HybridIDE.exe created successfully!
    echo.
    echo File size:
    dir HybridIDE.exe | find "HybridIDE.exe"
    echo.
    echo ========================================
    echo        BUILD COMPLETE!
    echo ========================================
    echo.
    echo You can now run: HybridIDE.exe
    echo.
) else (
    echo ERROR: HybridIDE.exe was not created
    pause
    exit /b 1
)

pause
