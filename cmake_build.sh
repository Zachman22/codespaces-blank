#!/bin/bash
# CMake Build Script for Hybrid IDE
# Cross-platform build with automatic dependency detection

set -e

echo "========================================"
echo "  Hybrid IDE - CMake Build"
echo "========================================"
echo ""

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}[ERROR]${NC} CMake not found. Please install CMake 3.15 or later."
    echo "  Ubuntu/Debian: sudo apt install cmake"
    echo "  macOS: brew install cmake"
    echo "  Windows: Download from https://cmake.org/download/"
    exit 1
fi

echo -e "${GREEN}[OK]${NC} CMake found: $(cmake --version | head -n1)"

# Check for C++ compiler
if command -v g++ &> /dev/null; then
    echo -e "${GREEN}[OK]${NC} g++ found: $(g++ --version | head -n1)"
elif command -v clang++ &> /dev/null; then
    echo -e "${GREEN}[OK]${NC} clang++ found: $(clang++ --version | head -n1)"
elif command -v cl &> /dev/null; then
    echo -e "${GREEN}[OK]${NC} MSVC found"
else
    echo -e "${RED}[ERROR]${NC} No C++ compiler found. Please install:"
    echo "  Ubuntu/Debian: sudo apt install build-essential"
    echo "  macOS: xcode-select --install"
    echo "  Windows: Install MinGW-w64 or Visual Studio"
    exit 1
fi

# Check for Node.js
if ! command -v node &> /dev/null; then
    echo -e "${YELLOW}[WARNING]${NC} Node.js not found. Frontend will not be built."
    echo "  Install from: https://nodejs.org/"
    BUILD_FRONTEND=0
else
    echo -e "${GREEN}[OK]${NC} Node.js found: $(node --version)"
    BUILD_FRONTEND=1
fi

echo ""

# Build frontend if Node.js is available
if [ $BUILD_FRONTEND -eq 1 ]; then
    echo -e "${CYAN}[1/3]${NC} Building React frontend..."
    cd frontend
    
    if [ ! -d "node_modules" ]; then
        echo "Installing npm dependencies..."
        npm install
    fi
    
    echo "Building frontend with Vite..."
    npm run build
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}[ERROR]${NC} Frontend build failed"
        exit 1
    fi
    
    cd ..
    echo -e "${GREEN}[OK]${NC} Frontend build complete"
    echo ""
fi

# Create build directory
echo -e "${CYAN}[2/3]${NC} Configuring CMake..."
rm -rf build
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR]${NC} CMake configuration failed"
    exit 1
fi

echo -e "${GREEN}[OK]${NC} CMake configuration complete"
echo ""

# Build
echo -e "${CYAN}[3/3]${NC} Building C++ backend..."
cmake --build . --config Release

if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR]${NC} Build failed"
    exit 1
fi

echo -e "${GREEN}[OK]${NC} Build complete"
echo ""

# Summary
echo "========================================"
echo -e "${GREEN}  Build Complete!${NC}"
echo "========================================"
echo ""
echo "Executable: build/bin/HybridIDE"
echo ""
echo "To install:"
echo "  sudo cmake --install build"
echo ""
echo "To create installer package:"
echo "  cd build && cpack"
echo ""
echo "To run:"
echo "  ./build/bin/HybridIDE"
echo ""
