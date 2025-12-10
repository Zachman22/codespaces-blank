#!/bin/bash
# Quick launch script for Script Generator Chatbot

echo "🚀 Starting Script Generator Chatbot..."

# Check if build exists
if [ ! -f "build/bin/ScriptGeneratorChatbot" ]; then
    echo "📦 Building ScriptGeneratorChatbot..."
    
    # Configure if needed
    if [ ! -f "build/CMakeCache.txt" ]; then
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    fi
    
    # Build
    cmake --build build --target ScriptGeneratorChatbot
    
    if [ $? -ne 0 ]; then
        echo "❌ Build failed!"
        exit 1
    fi
    
    echo "✅ Build complete!"
fi

# Check if executable exists
if [ ! -f "build/bin/ScriptGeneratorChatbot" ]; then
    echo "❌ Executable not found!"
    exit 1
fi

# Run the chatbot
./build/bin/ScriptGeneratorChatbot
