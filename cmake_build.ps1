# CMake Build Script for Hybrid IDE (Windows PowerShell)
# Automatic dependency detection and configuration

param(
    [switch]$Debug,
    [switch]$Clean,
    [switch]$Install,
    [switch]$Package,
    [string]$Generator = "MinGW Makefiles"
)

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Hybrid IDE - CMake Build" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check for CMake
try {
    $cmakeVersion = cmake --version 2>&1 | Select-Object -First 1
    Write-Host "[OK] CMake found: $cmakeVersion" -ForegroundColor Green
} catch {
    Write-Host "[ERROR] CMake not found. Please install CMake 3.15 or later." -ForegroundColor Red
    Write-Host "  Download from: https://cmake.org/download/" -ForegroundColor Yellow
    exit 1
}

# Check for C++ compiler
$compilerFound = $false
if (Get-Command g++ -ErrorAction SilentlyContinue) {
    $gccVersion = g++ --version 2>&1 | Select-Object -First 1
    Write-Host "[OK] g++ found: $gccVersion" -ForegroundColor Green
    $compilerFound = $true
} elseif (Get-Command cl -ErrorAction SilentlyContinue) {
    Write-Host "[OK] MSVC found" -ForegroundColor Green
    $Generator = "Visual Studio 17 2022"
    $compilerFound = $true
}

if (-not $compilerFound) {
    Write-Host "[ERROR] No C++ compiler found. Please install:" -ForegroundColor Red
    Write-Host "  MinGW-w64: https://www.mingw-w64.org/" -ForegroundColor Yellow
    Write-Host "  Visual Studio: https://visualstudio.microsoft.com/" -ForegroundColor Yellow
    exit 1
}

# Check for Node.js
$buildFrontend = $true
try {
    $nodeVersion = node --version
    Write-Host "[OK] Node.js found: $nodeVersion" -ForegroundColor Green
} catch {
    Write-Host "[WARNING] Node.js not found. Frontend will not be built." -ForegroundColor Yellow
    Write-Host "  Install from: https://nodejs.org/" -ForegroundColor Yellow
    $buildFrontend = $false
}

Write-Host ""

# Clean if requested
if ($Clean) {
    Write-Host "Cleaning build artifacts..." -ForegroundColor Yellow
    if (Test-Path "build") {
        Remove-Item -Recurse -Force "build"
    }
    Write-Host "[OK] Clean complete" -ForegroundColor Green
    Write-Host ""
}

# Build frontend
if ($buildFrontend) {
    Write-Host "[1/3] Building React frontend..." -ForegroundColor Cyan
    Push-Location frontend
    
    try {
        if (-not (Test-Path "node_modules")) {
            Write-Host "Installing npm dependencies..." -ForegroundColor Yellow
            npm install
        }
        
        Write-Host "Building frontend with Vite..." -ForegroundColor Yellow
        npm run build
        
        if ($LASTEXITCODE -ne 0) {
            throw "Frontend build failed"
        }
        
        Write-Host "[OK] Frontend build complete" -ForegroundColor Green
    } catch {
        Write-Host "[ERROR] Frontend build failed: $_" -ForegroundColor Red
        Pop-Location
        exit 1
    } finally {
        Pop-Location
    }
    
    Write-Host ""
}

# Create build directory
Write-Host "[2/3] Configuring CMake..." -ForegroundColor Cyan
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Push-Location build

try {
    # Determine build type
    $buildType = if ($Debug) { "Debug" } else { "Release" }
    
    # Configure with CMake
    Write-Host "Generator: $Generator" -ForegroundColor Yellow
    Write-Host "Build Type: $buildType" -ForegroundColor Yellow
    
    cmake .. -G "$Generator" -DCMAKE_BUILD_TYPE=$buildType
    
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed"
    }
    
    Write-Host "[OK] CMake configuration complete" -ForegroundColor Green
    Write-Host ""
    
    # Build
    Write-Host "[3/3] Building C++ backend..." -ForegroundColor Cyan
    cmake --build . --config $buildType
    
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }
    
    Write-Host "[OK] Build complete" -ForegroundColor Green
    Write-Host ""
    
    # Install if requested
    if ($Install) {
        Write-Host "Installing..." -ForegroundColor Cyan
        cmake --install . --config $buildType
        
        if ($LASTEXITCODE -ne 0) {
            throw "Installation failed"
        }
        
        Write-Host "[OK] Installation complete" -ForegroundColor Green
        Write-Host ""
    }
    
    # Package if requested
    if ($Package) {
        Write-Host "Creating installer package..." -ForegroundColor Cyan
        cpack -C $buildType
        
        if ($LASTEXITCODE -ne 0) {
            throw "Packaging failed"
        }
        
        Write-Host "[OK] Package created" -ForegroundColor Green
        Write-Host ""
    }
    
} catch {
    Write-Host "[ERROR] $_" -ForegroundColor Red
    Pop-Location
    exit 1
} finally {
    Pop-Location
}

# Summary
Write-Host "========================================" -ForegroundColor Green
Write-Host "  Build Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Executable: build\bin\HybridIDE.exe" -ForegroundColor Cyan
Write-Host ""
Write-Host "To install:" -ForegroundColor Yellow
Write-Host "  cmake --install build" -ForegroundColor White
Write-Host ""
Write-Host "To create installer package:" -ForegroundColor Yellow
Write-Host "  cd build && cpack" -ForegroundColor White
Write-Host ""
Write-Host "To run:" -ForegroundColor Yellow
Write-Host "  .\build\bin\HybridIDE.exe" -ForegroundColor White
Write-Host ""
