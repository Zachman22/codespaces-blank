# Build script for Hybrid IDE
# Builds both frontend and backend

param(
    [switch]$Clean,
    [switch]$Frontend,
    [switch]$Backend,
    [switch]$Release,
    [switch]$Debug = $true
)

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Hybrid IDE Build Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Get script directory
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RootDir = $ScriptDir

# Check for required tools
function Test-Tool {
    param([string]$Command)
    try {
        $null = Get-Command $Command -ErrorAction Stop
        return $true
    } catch {
        return $false
    }
}

Write-Host "Checking for required tools..." -ForegroundColor Yellow

if (-not (Test-Tool "g++")) {
    Write-Host "[ERROR] MinGW g++ not found. Please install MinGW-w64 and add to PATH." -ForegroundColor Red
    exit 1
}

if (-not (Test-Tool "node")) {
    Write-Host "[ERROR] Node.js not found. Please install Node.js." -ForegroundColor Red
    exit 1
}

if (-not (Test-Tool "npm")) {
    Write-Host "[ERROR] npm not found. Please install Node.js with npm." -ForegroundColor Red
    exit 1
}

Write-Host "[OK] All required tools found" -ForegroundColor Green
Write-Host ""

# Clean if requested
if ($Clean) {
    Write-Host "Cleaning build artifacts..." -ForegroundColor Yellow
    
    if (Test-Path "$RootDir\build") {
        Remove-Item -Recurse -Force "$RootDir\build"
    }
    if (Test-Path "$RootDir\output") {
        Remove-Item -Recurse -Force "$RootDir\output"
    }
    if (Test-Path "$RootDir\frontend\dist") {
        Remove-Item -Recurse -Force "$RootDir\frontend\dist"
    }
    
    Write-Host "[OK] Clean complete" -ForegroundColor Green
    Write-Host ""
}

# Build frontend by default or if requested
if (-not $Backend -or $Frontend) {
    Write-Host "Building React frontend..." -ForegroundColor Yellow
    
    Push-Location "$RootDir\frontend"
    
    try {
        # Install dependencies if node_modules doesn't exist
        if (-not (Test-Path "node_modules")) {
            Write-Host "Installing npm dependencies..." -ForegroundColor Yellow
            npm install
        }
        
        # Build frontend
        Write-Host "Building frontend with Vite..." -ForegroundColor Yellow
        npm run build
        
        if ($LASTEXITCODE -ne 0) {
            throw "Frontend build failed"
        }
        
        Write-Host "[OK] Frontend build complete" -ForegroundColor Green
    } finally {
        Pop-Location
    }
    
    Write-Host ""
}

# Build backend by default or if requested
if (-not $Frontend -or $Backend) {
    Write-Host "Building C++ backend..." -ForegroundColor Yellow
    
    # Create directories
    if (-not (Test-Path "$RootDir\build")) {
        New-Item -ItemType Directory -Path "$RootDir\build" | Out-Null
    }
    if (-not (Test-Path "$RootDir\output")) {
        New-Item -ItemType Directory -Path "$RootDir\output" | Out-Null
    }
    
    # Compiler settings
    $CXX = "g++"
    $CXXFLAGS = @(
        "-std=c++17",
        "-Wall",
        "-Wextra"
    )
    
    if ($Release) {
        $CXXFLAGS += "-O2"
        Write-Host "Building in Release mode" -ForegroundColor Cyan
    } else {
        $CXXFLAGS += @("-O0", "-g")
        Write-Host "Building in Debug mode" -ForegroundColor Cyan
    }
    
    # WebView2 paths - adjust these based on your installation
    $WebView2Include = "C:\Program Files (x86)\Microsoft\WebView2\Include"
    $WebView2Lib = "C:\Program Files (x86)\Microsoft\WebView2\lib\x64"
    
    # Check if WebView2 SDK exists
    if (-not (Test-Path $WebView2Include)) {
        Write-Host "[WARNING] WebView2 SDK not found at default location" -ForegroundColor Yellow
        Write-Host "Please install WebView2 SDK or update paths in build.ps1" -ForegroundColor Yellow
        Write-Host "Continuing with minimal includes..." -ForegroundColor Yellow
        $WebView2Include = ""
        $WebView2Lib = ""
    }
    
    $INCLUDES = @("-Iinclude")
    if ($WebView2Include) {
        $INCLUDES += "-I`"$WebView2Include`""
    }
    
    $LIBS = @(
        "-lole32",
        "-loleaut32",
        "-lshlwapi",
        "-luuid",
        "-lwindowscodecs"
    )
    
    $LDFLAGS = @()
    if ($WebView2Lib) {
        $LDFLAGS += "-L`"$WebView2Lib`""
    }
    
    # Source files - include code_signer.cpp
    $SourceFiles = Get-ChildItem -Path "$RootDir\src" -Filter "*.cpp"
    $ObjectFiles = @()
    
    # Add OpenSSL and crypto libraries for code signing
    $LIBS += @("-lcrypt32", "-lssl", "-lcrypto")
    
    # Compile each source file
    foreach ($SourceFile in $SourceFiles) {
        $ObjectFile = "$RootDir\build\$($SourceFile.BaseName).o"
        $ObjectFiles += $ObjectFile
        
        Write-Host "Compiling $($SourceFile.Name)..." -ForegroundColor Yellow
        
        $CompileCmd = @($CXX) + $CXXFLAGS + $INCLUDES + @("-c", $SourceFile.FullName, "-o", $ObjectFile)
        
        & $CompileCmd[0] $CompileCmd[1..($CompileCmd.Length-1)]
        
        if ($LASTEXITCODE -ne 0) {
            Write-Host "[ERROR] Compilation failed for $($SourceFile.Name)" -ForegroundColor Red
            exit 1
        }
    }
    
    # Link
    Write-Host "Linking..." -ForegroundColor Yellow
    
    $LinkCmd = @($CXX) + $ObjectFiles + @("-o", "$RootDir\output\HybridIDE.exe") + $LDFLAGS + $LIBS + @("-mwindows")
    
    & $LinkCmd[0] $LinkCmd[1..($LinkCmd.Length-1)]
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "[ERROR] Linking failed" -ForegroundColor Red
        exit 1
    }
    
    Write-Host "[OK] Backend build complete" -ForegroundColor Green
    Write-Host ""
}

# Copy frontend to output
if (Test-Path "$RootDir\frontend\dist") {
    Write-Host "Copying frontend to output..." -ForegroundColor Yellow
    
    if (Test-Path "$RootDir\output\frontend") {
        Remove-Item -Recurse -Force "$RootDir\output\frontend"
    }
    
    Copy-Item -Recurse "$RootDir\frontend\dist" "$RootDir\output\frontend"
    
    Write-Host "[OK] Frontend copied to output" -ForegroundColor Green
    Write-Host ""
}

Write-Host "========================================" -ForegroundColor Green
Write-Host "  Build Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Output: $RootDir\output\HybridIDE.exe" -ForegroundColor Cyan
Write-Host ""
Write-Host "To run the IDE:" -ForegroundColor Yellow
Write-Host "  cd output" -ForegroundColor White
Write-Host "  .\HybridIDE.exe" -ForegroundColor White
Write-Host ""
