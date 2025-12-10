# Package script for Hybrid IDE
# Creates a distributable package

param(
    [string]$OutputName = "HybridIDE-Portable",
    [string]$Version = "1.0.0"
)

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Hybrid IDE Package Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RootDir = $ScriptDir
$PackageDir = "$RootDir\package\$OutputName-$Version"

# Check if build exists
if (-not (Test-Path "$RootDir\output\HybridIDE.exe")) {
    Write-Host "[ERROR] HybridIDE.exe not found. Please build first." -ForegroundColor Red
    Write-Host "Run: .\build.ps1" -ForegroundColor Yellow
    exit 1
}

# Create package directory
Write-Host "Creating package directory..." -ForegroundColor Yellow

if (Test-Path "$RootDir\package") {
    Remove-Item -Recurse -Force "$RootDir\package"
}

New-Item -ItemType Directory -Path $PackageDir | Out-Null

# Copy executable
Write-Host "Copying executable..." -ForegroundColor Yellow
Copy-Item "$RootDir\output\HybridIDE.exe" "$PackageDir\"

# Copy frontend
if (Test-Path "$RootDir\output\frontend") {
    Write-Host "Copying frontend..." -ForegroundColor Yellow
    Copy-Item -Recurse "$RootDir\output\frontend" "$PackageDir\"
}

# Copy README
if (Test-Path "$RootDir\README.md") {
    Write-Host "Copying README..." -ForegroundColor Yellow
    Copy-Item "$RootDir\README.md" "$PackageDir\"
}

# Create config file
Write-Host "Creating default config..." -ForegroundColor Yellow

$ConfigContent = @"
{
  "version": "$Version",
  "compiler": {
    "path": "g++",
    "flags": ["-std=c++17", "-Wall"],
    "optimization": "O2"
  },
  "editor": {
    "fontSize": 14,
    "theme": "vs-dark",
    "tabSize": 4
  },
  "build": {
    "autoBuild": false,
    "showWarnings": true
  }
}
"@

$ConfigContent | Out-File -FilePath "$PackageDir\config.json" -Encoding UTF8

# Create launcher script
Write-Host "Creating launcher..." -ForegroundColor Yellow

$LauncherContent = @"
@echo off
echo Starting Hybrid IDE...
echo.

REM Check for MinGW
where g++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [WARNING] MinGW g++ not found in PATH
    echo Please install MinGW-w64 for build functionality
    echo.
)

REM Check for WebView2
reg query "HKLM\SOFTWARE\WOW6432Node\Microsoft\EdgeUpdate\Clients\{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}" >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [WARNING] WebView2 Runtime not found
    echo Please install WebView2 Runtime from:
    echo https://developer.microsoft.com/en-us/microsoft-edge/webview2/
    echo.
    pause
)

start "" HybridIDE.exe
"@

$LauncherContent | Out-File -FilePath "$PackageDir\Start.bat" -Encoding ASCII

# Create installation instructions
Write-Host "Creating installation guide..." -ForegroundColor Yellow

$InstallContent = @"
# Hybrid IDE - Installation Guide

## Quick Start

1. Extract this folder anywhere on your system
2. Run 'Start.bat' to launch the IDE

## Requirements

### Required:
- Windows 10/11 (x64)
- WebView2 Runtime (usually pre-installed on Windows 11)

### For Build Functionality:
- MinGW-w64 GCC compiler
- Add MinGW bin directory to system PATH

## Installing MinGW-w64

### Method 1: MSYS2 (Recommended)
1. Download MSYS2 from https://www.msys2.org/
2. Install MSYS2
3. Open MSYS2 terminal and run:
   ```
   pacman -S mingw-w64-x86_64-gcc
   ```
4. Add to PATH: C:\msys64\mingw64\bin

### Method 2: Standalone Installer
1. Download from https://www.mingw-w64.org/downloads/
2. Install to C:\MinGW
3. Add to PATH: C:\MinGW\bin

## Installing WebView2 Runtime

If WebView2 is not already installed:
1. Download from: https://developer.microsoft.com/en-us/microsoft-edge/webview2/
2. Run the installer
3. Restart Hybrid IDE

## Configuration

Edit 'config.json' to customize:
- Compiler path and flags
- Editor settings (font size, theme, etc.)
- Build options

## Troubleshooting

### IDE won't start
- Install WebView2 Runtime
- Check Windows version (10/11 required)

### Build button doesn't work
- Install MinGW-w64
- Add MinGW bin directory to PATH
- Restart IDE after PATH changes

### Can't find my files
- Use File > Open Folder to open your project
- Files will appear in the Explorer panel

## Support

For issues, visit: https://github.com/yourusername/hybrid-ide

## Version

Version: $Version
Build Date: $(Get-Date -Format "yyyy-MM-dd")
"@

$InstallContent | Out-File -FilePath "$PackageDir\INSTALL.md" -Encoding UTF8

# Create archive
Write-Host "Creating ZIP archive..." -ForegroundColor Yellow

$ZipPath = "$RootDir\package\$OutputName-$Version.zip"

Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::CreateFromDirectory(
    $PackageDir,
    $ZipPath,
    [System.IO.Compression.CompressionLevel]::Optimal,
    $false
)

# Summary
Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  Package Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Package created:" -ForegroundColor Cyan
Write-Host "  Folder: $PackageDir" -ForegroundColor White
Write-Host "  Archive: $ZipPath" -ForegroundColor White
Write-Host ""
Write-Host "Contents:" -ForegroundColor Yellow
Write-Host "  - HybridIDE.exe" -ForegroundColor White
Write-Host "  - Frontend (React UI)" -ForegroundColor White
Write-Host "  - Start.bat (Launcher)" -ForegroundColor White
Write-Host "  - config.json (Configuration)" -ForegroundColor White
Write-Host "  - README.md (Documentation)" -ForegroundColor White
Write-Host "  - INSTALL.md (Setup Guide)" -ForegroundColor White
Write-Host ""
Write-Host "Package size: $([math]::Round((Get-ChildItem $ZipPath).Length / 1MB, 2)) MB" -ForegroundColor Cyan
Write-Host ""
