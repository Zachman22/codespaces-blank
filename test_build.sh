#!/bin/bash

echo "======================================"
echo "HYBRID IDE - BUILD TEST SCRIPT"
echo "======================================"
echo ""

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Navigate to frontend directory
echo "📁 Navigating to frontend directory..."
cd frontend || exit 1

# Check if node_modules exists
if [ -d "node_modules" ]; then
    echo -e "${GREEN}✓${NC} Node modules already installed"
else
    echo -e "${YELLOW}⚠${NC} Installing node modules..."
    npm install
fi

echo ""
echo "🔨 Building React frontend..."
echo "======================================"

npm run build

if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}✓ Frontend build successful!${NC}"
    echo ""
    echo "📊 Build statistics:"
    if [ -d "dist" ]; then
        echo "  Output directory: dist/"
        ls -lh dist/
    fi
else
    echo ""
    echo -e "${RED}✗ Frontend build failed!${NC}"
    exit 1
fi

echo ""
echo "======================================"
echo "✨ Build test complete!"
echo "======================================"
