# Quick Start Guide

## Installation

1. **Download** the Hybrid IDE package
2. **Extract** to any location (e.g., `C:\HybridIDE\`)
3. **Install prerequisites** (see below)
4. **Run** `Start.bat` or `HybridIDE.exe`

## Prerequisites

### Required (for IDE to start):
- **Windows 10/11** (64-bit)
- **WebView2 Runtime** (usually pre-installed on Windows 11)
  - Download: https://go.microsoft.com/fwlink/p/?LinkId=2124703

### Required (for building code):
- **MinGW-w64** (GCC compiler for Windows)
  
  **Option 1: MSYS2 (Recommended)**
  ```powershell
  # Download installer from https://www.msys2.org/
  # After installation, open MSYS2 terminal and run:
  pacman -S mingw-w64-x86_64-gcc
  
  # Add to PATH:
  # C:\msys64\mingw64\bin
  ```
  
  **Option 2: Standalone MinGW**
  ```powershell
  # Download from https://winlibs.com/ or https://www.mingw-w64.org/
  # Extract to C:\MinGW
  # Add to PATH:
  # C:\MinGW\bin
  ```

## First Build

### Step 1: Open the IDE
Double-click `Start.bat` or `HybridIDE.exe`

### Step 2: Create/Open a C++ File
- Use the File Explorer panel on the left
- Click on `examples/hello_world.cpp`
- Or create a new file with `.cpp` extension

### Step 3: Build
1. Click the **Build** button in the toolbar (or press `Ctrl+B`)
2. Watch the build output in the console panel
3. If successful, an `.exe` file will be created

### Step 4: Run
1. Click the **Run** button (or press `Ctrl+F5`)
2. See program output in the console panel

## Example Project

Try building the included example:

```cpp
// hello_world.cpp
#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    std::cout << "Welcome to Hybrid IDE!" << std::endl;
    return 0;
}
```

**Build command:** Click "Build" button
**Output:** `hello_world.exe`
**Run:** Click "Run" button

## IDE Features

### File Explorer
- Navigate project files
- Click to open in editor
- Expand/collapse folders

### Monaco Editor
- Syntax highlighting
- Code completion (basic)
- Line numbers
- Minimap
- Multiple tabs

### Toolbar
- **Build** - Compile current file
- **Run** - Execute compiled program
- **Debug** - (Coming soon) GDB integration

### Build Panel
- Real-time compilation logs
- Error/warning highlighting
- Success/failure messages
- Clear button to reset

### AI Sidebar
- Coding assistant (placeholder)
- Ask questions about code
- Get suggestions

### System Info
- View OS version
- Check CPU and RAM
- Verify architecture (x64/ARM64)

## Common Issues

### "IDE won't start"
**Problem:** WebView2 not installed
**Solution:** Install WebView2 Runtime from Microsoft
```
https://go.microsoft.com/fwlink/p/?LinkId=2124703
```

### "Build button does nothing"
**Problem:** MinGW not installed or not in PATH
**Solution:**
1. Install MinGW-w64
2. Add to system PATH
3. Verify: Open Command Prompt and run `g++ --version`
4. Restart IDE

### "File not found" error
**Problem:** Invalid file path
**Solution:** Check that the source file exists and path is correct

### "Compilation failed"
**Problem:** Syntax errors in code
**Solution:** Check the build panel for error messages

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+B` | Build current file |
| `Ctrl+F5` | Run program |
| `F5` | Debug (coming soon) |
| `Ctrl+S` | Save file |
| `Ctrl+W` | Close tab |
| `Ctrl+Tab` | Switch tabs |

## Build Options

The IDE uses these default compiler flags:
```
-std=c++17      # C++17 standard
-Wall           # All warnings
-O0             # No optimization (Debug)
-g              # Debug symbols
```

To customize, edit `config.json`:
```json
{
  "compiler": {
    "flags": ["-std=c++20", "-O2", "-Wall"]
  }
}
```

## Advanced Usage

### Custom Build Flags
Edit source file path and add custom flags in the build dialog (feature coming soon)

### Include Directories
Add include paths in `config.json`:
```json
{
  "paths": {
    "includeDirectories": ["include", "lib/headers"]
  }
}
```

### Linking Libraries
Modify compiler flags to link libraries:
```
-lmylib
```

## Getting Help

### Documentation
- See `README.md` for full documentation
- See `DOCUMENTATION.md` for developer guide

### Troubleshooting
- Check build logs for errors
- Verify MinGW installation
- Check file paths
- Ensure WebView2 is installed

### Support
- GitHub: https://github.com/yourusername/hybrid-ide
- Issues: Report bugs and request features

## Next Steps

1. **Explore Examples**
   - Try `examples/test_program.cpp`
   - Experiment with STL, classes, templates

2. **Customize Settings**
   - Edit `config.json`
   - Change theme, font size, etc.

3. **Build Your Project**
   - Create new `.cpp` files
   - Organize in folders
   - Build and run

4. **Learn More**
   - Read full documentation
   - Explore C++ tutorials
   - Try advanced features

## Tips

- Save your work frequently
- Use meaningful file names
- Organize code in folders
- Check build logs for warnings
- Keep MinGW updated

## Updates

To update the IDE:
1. Download latest version
2. Extract to new folder
3. Copy your projects
4. Copy `config.json` (if customized)

---

**Welcome to Hybrid IDE!** 🚀

Happy coding! 💻
