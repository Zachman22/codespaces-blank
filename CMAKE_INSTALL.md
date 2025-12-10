# Hybrid IDE - CMake Installation Guide

## Quick Start

### Windows - One Command Install

```batch
cmake_build.bat
```

Or with PowerShell:
```powershell
.\cmake_build.ps1
```

### Linux/macOS - One Command Install

```bash
chmod +x cmake_build.sh
./cmake_build.sh
```

## Detailed Installation

### Prerequisites

#### Required
- **CMake 3.15+** - Build system generator
- **C++ Compiler** - g++, clang++, or MSVC
- **Node.js 18+** - For building the frontend

#### Optional
- **OpenSSL** - For code signing (falls back if not available)
- **NSIS** - For creating Windows installers

### Install Prerequisites

#### Windows
```powershell
# Install CMake
choco install cmake

# Install MinGW-w64
choco install mingw

# Install Node.js
choco install nodejs

# Install OpenSSL (optional)
choco install openssl
```

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install cmake build-essential nodejs npm libssl-dev
```

#### macOS
```bash
brew install cmake node openssl
```

## Build Methods

### Method 1: Automated Scripts (Recommended)

**Windows Batch:**
```batch
cmake_build.bat
```

**Windows PowerShell:**
```powershell
# Basic build
.\cmake_build.ps1

# Debug build
.\cmake_build.ps1 -Debug

# Clean and build
.\cmake_build.ps1 -Clean

# Build and install
.\cmake_build.ps1 -Install

# Build and create installer
.\cmake_build.ps1 -Package
```

**Linux/macOS:**
```bash
./cmake_build.sh
```

### Method 2: Manual CMake

```bash
# Build frontend
cd frontend
npm install
npm run build
cd ..

# Configure CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release

# Install (optional)
sudo cmake --install .

# Create package (optional)
cpack
```

### Method 3: With Ninja (Faster Builds)

```bash
# Install Ninja
# Windows: choco install ninja
# Ubuntu: sudo apt install ninja-build
# macOS: brew install ninja

# Configure with Ninja
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build
ninja

# Install
sudo ninja install
```

## Build Options

### Build Types

```bash
# Debug build (with debug symbols)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build (optimized)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Release with debug info
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Minimum size release
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
```

### Generators

```bash
# MinGW Makefiles (Windows with MinGW)
cmake .. -G "MinGW Makefiles"

# Visual Studio (Windows with MSVC)
cmake .. -G "Visual Studio 17 2022"

# Ninja (Fast, cross-platform)
cmake .. -G Ninja

# Unix Makefiles (Linux/macOS)
cmake .. -G "Unix Makefiles"
```

### Custom Install Location

```bash
# Configure with custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/HybridIDE

# Install to custom location
cmake --install . --prefix /opt/HybridIDE
```

## Installation

### System-wide Installation

```bash
# Linux/macOS
sudo cmake --install build

# Windows (as Administrator)
cmake --install build
```

**Installed files:**
- `/usr/local/bin/HybridIDE` - Executable
- `/usr/local/bin/frontend/` - Web UI files
- `/usr/local/examples/` - Example C++ programs
- `/usr/local/README.md` - Documentation

### Portable Installation

```bash
# Install to local directory
cmake --install build --prefix ./install

# Run from local directory
./install/bin/HybridIDE
```

## Creating Installers

### Windows Installer (NSIS)

```powershell
# Install NSIS
choco install nsis

# Build and create installer
cd build
cpack -G NSIS

# Output: HybridIDE-1.0.0-win64.exe
```

### Windows ZIP Package

```bash
cd build
cpack -G ZIP

# Output: HybridIDE-1.0.0-win64.zip
```

### Linux Packages

```bash
cd build

# DEB package (Debian/Ubuntu)
cpack -G DEB

# RPM package (Fedora/RHEL)
cpack -G RPM

# TGZ archive
cpack -G TGZ
```

### macOS Package

```bash
cd build

# DMG image
cpack -G DragNDrop

# TGZ archive
cpack -G TGZ
```

## Troubleshooting

### CMake Not Found

**Error:** `cmake: command not found`

**Solution:**
```bash
# Windows
choco install cmake

# Ubuntu/Debian
sudo apt install cmake

# macOS
brew install cmake
```

### Compiler Not Found

**Error:** `No CMAKE_CXX_COMPILER could be found`

**Solution:**
```bash
# Windows - Install MinGW
choco install mingw

# Ubuntu/Debian
sudo apt install build-essential

# macOS
xcode-select --install
```

### OpenSSL Not Found

**Warning:** `OpenSSL not found - code signing may use fallback methods`

**Solution (optional):**
```bash
# Windows
choco install openssl

# Ubuntu/Debian
sudo apt install libssl-dev

# macOS
brew install openssl

# Then reconfigure
cmake .. -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl
```

### Frontend Build Fails

**Error:** `npm: command not found`

**Solution:**
```bash
# Install Node.js
# Windows: choco install nodejs
# Ubuntu: sudo apt install nodejs npm
# macOS: brew install node

# Then rebuild
cd frontend
npm install
npm run build
```

### Permission Denied on Install

**Error:** `Permission denied`

**Solution:**
```bash
# Linux/macOS - Use sudo
sudo cmake --install build

# Windows - Run as Administrator
# Right-click PowerShell → Run as Administrator
cmake --install build
```

### Visual Studio Generator Issues

**Error:** `Could not find Visual Studio`

**Solutions:**
```powershell
# Option 1: Use MinGW instead
cmake .. -G "MinGW Makefiles"

# Option 2: Install Visual Studio
# Download from: https://visualstudio.microsoft.com/

# Option 3: Specify VS version
cmake .. -G "Visual Studio 16 2019"
```

## Advanced Configuration

### Cross-Compilation

**Windows to Windows (on Linux):**
```bash
# Install MinGW cross-compiler
sudo apt install mingw-w64

# Configure for Windows target
cmake .. \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
  -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++

# Build
cmake --build .
```

### Custom Compiler

```bash
# Use specific compiler
cmake .. \
  -DCMAKE_C_COMPILER=/usr/bin/gcc-11 \
  -DCMAKE_CXX_COMPILER=/usr/bin/g++-11
```

### Static Linking (Portable EXE)

```bash
# Configure for static linking
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF
```

### Parallel Builds

```bash
# Build with multiple cores
cmake --build . -- -j8

# Or with Ninja
ninja -j8
```

## Verification

### Check Build

```bash
# Verify executable exists
ls build/bin/HybridIDE*

# Check dependencies (Linux)
ldd build/bin/HybridIDE

# Check dependencies (Windows)
dumpbin /dependents build/bin/HybridIDE.exe
```

### Test Installation

```bash
# Run from build directory
./build/bin/HybridIDE

# Run after installation
HybridIDE
```

### Verify Package

```bash
# List package contents (Windows)
7z l HybridIDE-1.0.0-win64.zip

# List package contents (Linux)
dpkg -c HybridIDE-1.0.0-Linux.deb
```

## Uninstallation

```bash
# From build directory
cd build
cmake --build . --target uninstall

# Or manually remove files
# Linux/macOS
sudo rm -rf /usr/local/bin/HybridIDE /usr/local/bin/frontend

# Windows
# Use Add/Remove Programs or delete install directory
```

## Summary

**Easiest way to install:**
```bash
# Windows
cmake_build.bat

# Linux/macOS  
./cmake_build.sh
```

**Then run:**
```bash
./build/bin/HybridIDE
```

That's it! Your IDE is ready to use! 🚀
