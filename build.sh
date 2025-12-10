#!/bin/bash
# Build script for Linux (cross-compile for Windows)

set -e

echo "========================================"
echo "  Hybrid IDE Build Script (Linux)"
echo "========================================"
echo ""

# Check for MinGW cross-compiler
if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    echo "[ERROR] MinGW cross-compiler not found"
    echo "Install with: sudo apt-get install mingw-w64"
    exit 1
fi

# Check for Node.js
if ! command -v node &> /dev/null; then
    echo "[ERROR] Node.js not found"
    echo "Install from: https://nodejs.org/"
    exit 1
fi

echo "[OK] All required tools found"
echo ""

# Build frontend
echo "Building React frontend..."
cd frontend

if [ ! -d "node_modules" ]; then
    echo "Installing npm dependencies..."
    npm install
fi

echo "Building frontend with Vite..."
npm run build

cd ..

echo "[OK] Frontend build complete"
echo ""

# Build backend
echo "Building C++ backend..."

mkdir -p build
mkdir -p output

CXX="x86_64-w64-mingw32-g++"
CXXFLAGS="-std=c++17 -Wall -Wextra -O2"
INCLUDES="-Iinclude"
LIBS="-lole32 -loleaut32 -lshlwapi -luuid -lwindowscodecs -lcrypt32 -lssl -lcrypto"

# Compile source files
for src in src/*.cpp; do
    obj="build/$(basename ${src%.cpp}.o)"
    echo "Compiling $(basename $src)..."
    $CXX $CXXFLAGS $INCLUDES -c $src -o $obj
done

# Link
echo "Linking..."
$CXX build/*.o -o output/HybridIDE.exe $LIBS -mwindows

echo "[OK] Backend build complete"
echo ""

# Copy frontend
echo "Copying frontend to output..."
cp -r frontend/dist output/frontend

echo ""
echo "========================================"
echo "  Build Complete!"
echo "========================================"
echo ""
echo "Output: output/HybridIDE.exe"
echo ""
