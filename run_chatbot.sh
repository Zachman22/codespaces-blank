#!/bin/bash

echo "=========================================="
echo "   AutoCoder Chatbot - Quick Start"
echo "=========================================="
echo ""

# Check if already built
if [ -f "build/bin/AutoCoderChatbot" ]; then
    echo "✓ AutoCoder Chatbot found!"
    ls -lh build/bin/AutoCoderChatbot
    echo ""
    echo "Starting chatbot..."
    echo ""
    ./build/bin/AutoCoderChatbot
else
    echo "Building AutoCoder Chatbot..."
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --target AutoCoderChatbot
    
    if [ -f "build/bin/AutoCoderChatbot" ]; then
        echo ""
        echo "✓ Build successful!"
        echo ""
        echo "Starting chatbot..."
        echo ""
        ./build/bin/AutoCoderChatbot
    else
        echo "✗ Build failed!"
        exit 1
    fi
fi
