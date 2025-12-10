@echo off
REM Launcher script for Hybrid IDE
REM Checks for prerequisites and starts the application

echo ========================================
echo   Hybrid IDE Launcher
echo ========================================
echo.

REM Check for MinGW
echo Checking for MinGW compiler...
where g++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [WARNING] MinGW g++ not found in PATH
    echo.
    echo The IDE will start, but you won't be able to build projects.
    echo.
    echo To enable building:
    echo   1. Install MinGW-w64 from https://www.mingw-w64.org/
    echo   2. Add MinGW bin directory to your system PATH
    echo   3. Restart this IDE
    echo.
    pause
) else (
    for /f "tokens=*" %%i in ('g++ --version 2^>nul') do (
        echo [OK] Found: %%i
        goto :mingw_ok
    )
)
:mingw_ok

echo.

REM Check for WebView2 Runtime
echo Checking for WebView2 Runtime...
reg query "HKLM\SOFTWARE\WOW6432Node\Microsoft\EdgeUpdate\Clients\{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}" >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [WARNING] WebView2 Runtime not found
    echo.
    echo The IDE requires WebView2 Runtime to run.
    echo.
    echo Download from:
    echo https://go.microsoft.com/fwlink/p/?LinkId=2124703
    echo.
    echo Press any key to continue anyway, or close this window to cancel.
    pause
) else (
    echo [OK] WebView2 Runtime is installed
)

echo.
echo Starting Hybrid IDE...
echo.

REM Start the application
if exist HybridIDE.exe (
    start "" HybridIDE.exe
    exit
) else (
    echo [ERROR] HybridIDE.exe not found!
    echo Please ensure you're running this script from the IDE installation directory.
    echo.
    pause
    exit /b 1
)
