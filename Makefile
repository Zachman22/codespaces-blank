# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -Iinclude -I"C:/Program Files (x86)/Microsoft/WebView2/Include"
LIBS = -lole32 -loleaut32 -lshlwapi -luuid -lwindowscodecs -lcrypt32 -lssl -lcrypto
LDFLAGS = -L"C:/Program Files (x86)/Microsoft/WebView2/lib/x64"

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
OUTPUT_DIR = output

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Target executable
TARGET = $(OUTPUT_DIR)/HybridIDE.exe

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@if not exist "$(OUTPUT_DIR)" mkdir "$(OUTPUT_DIR)"

# Build target executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(LIBS) -mwindows
	@echo Build complete: $(TARGET)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
	@if exist "$(OUTPUT_DIR)" rmdir /s /q "$(OUTPUT_DIR)"
	@echo Clean complete

# Rebuild
rebuild: clean all

# Run the application
run: $(TARGET)
	@cd $(OUTPUT_DIR) && HybridIDE.exe

# Install dependencies (placeholder)
install-deps:
	@echo Installing dependencies...
	@echo Please ensure MinGW-w64 and WebView2 SDK are installed
	@echo MinGW: https://www.mingw-w64.org/
	@echo WebView2: https://developer.microsoft.com/en-us/microsoft-edge/webview2/

.PHONY: all directories clean rebuild run install-deps
