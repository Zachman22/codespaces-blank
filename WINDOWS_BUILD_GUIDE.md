# 🪟 Windows 11 Build Guide - Hybrid IDE

## Complete Build Instructions for Windows 11

This guide will help you build the Hybrid IDE on Windows 11 with MinGW (no Visual Studio required).

---

## 📋 Prerequisites

### 1. Install MinGW-w64

**Option A: Using MSYS2 (Recommended)**
1. Download MSYS2 from: https://www.msys2.org/
2. Install and open MSYS2 MINGW64 terminal
3. Run:
```bash
pacman -Syu
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake make
```

**Option B: Standalone MinGW**
1. Download from: https://sourceforge.net/projects/mingw-w64/
2. Install to `C:\mingw64`
3. Add to PATH: `C:\mingw64\bin`

### 2. Install Node.js
1. Download from: https://nodejs.org/ (LTS version)
2. Run installer
3. Verify: `node --version` and `npm --version`

### 3. Install WebView2 Runtime
- Usually pre-installed on Windows 11
- If not: https://developer.microsoft.com/microsoft-edge/webview2/

---

## 🔨 Build Steps

### Step 1: Clone/Extract Project
```bash
cd C:\Projects
# Extract hybrid-ide folder here
cd hybrid-ide
```

### Step 2: Build React Frontend
```powershell
cd frontend
npm install
npm run build
```

**Expected Output:**
- `frontend/dist/` directory created
- `index.html`, `assets/` folder with JS/CSS bundles

### Step 3: Build C++ Backend
```powershell
# Return to project root
cd ..

# Option 1: Using PowerShell script
.\build.ps1

# Option 2: Using Make
make

# Option 3: Manual compilation
g++ -std=c++17 ^
    -I"include" ^
    -Ifrontend/dist ^
    src/main.cpp ^
    src/webview_host.cpp ^
    src/compiler.cpp ^
    src/system_info.cpp ^
    src/prompt_verifier.cpp ^
    -o HybridIDE.exe ^
    -lole32 -loleaut32 -luuid -lshell32 ^
    -mwindows
```

### Step 4: Run the IDE
```powershell
.\HybridIDE.exe
```

---

## 🎨 GUI Features

The IDE includes:

✅ **Modern Dark Theme** - GitHub-inspired color scheme
✅ **File Explorer** - Browse and open project files
✅ **Monaco Editor** - Full-featured code editor with syntax highlighting
✅ **Build System** - Integrated MinGW compiler
✅ **Build Panel** - Real-time compilation output
✅ **AI Assistant** - Coding help sidebar (ready for AI integration)
✅ **System Info** - Display OS, CPU, RAM information
✅ **Tab Management** - Multiple file tabs with modified indicators
✅ **Status Bar** - Build status and system information

---

## 🎯 Using the IDE

### Opening Files
1. Click on any file in the File Explorer (left panel)
2. File opens in a new tab in the editor
3. Edit code with syntax highlighting

### Building Code
1. Open a C/C++ source file (`.cpp`, `.c`, `.cc`)
2. Click **Build** button in toolbar
3. View compilation output in Build Panel (bottom)
4. Output `.exe` created in same directory as source

### Running Programs
1. After successful build, click **Run** button
2. Program executes and output appears in Build Panel
3. Watch for exit code in output

### AI Assistant
1. Click "AI Assistant" in status bar
2. Type coding questions or requests
3. Get help with code generation, debugging, refactoring
4. *(Note: In production, connect to GPT-4/Claude API)*

### System Information
1. Click system info display in status bar
2. View detailed OS, CPU, RAM information
3. Useful for platform-specific code

---

## 🔧 Customization

### Change Compiler Flags
Edit `config.json`:
```json
{
  "compiler": {
    "executable": "g++",
    "flags": [
      "-std=c++17",
      "-Wall",
      "-Wextra",
      "-O2"
    ]
  }
}
```

### Add File Types
Edit `frontend/src/components/FileExplorer.tsx`:
```typescript
const langMap: { [key: string]: string } = {
  'cpp': 'cpp',
  'py': 'python',
  'js': 'javascript',
  // Add your extensions here
};
```

### Modify Theme Colors
Edit `frontend/src/App.css`:
```css
:root {
  --bg-primary: #0d1117;
  --accent-blue: #58a6ff;
  /* Customize colors here */
}
```

---

## 📦 Distribution

### Create Portable Package
```powershell
# Create distribution folder
mkdir HybridIDE-Portable
cp HybridIDE.exe HybridIDE-Portable/
cp -r frontend/dist HybridIDE-Portable/webview
cp README.md HybridIDE-Portable/
cp LICENSE HybridIDE-Portable/

# Include MinGW (optional for end users)
cp -r C:\mingw64 HybridIDE-Portable/mingw64

# Zip it
Compress-Archive -Path HybridIDE-Portable -DestinationPath HybridIDE-Windows11-x64.zip
```

### Installer (Advanced)
Consider using:
- **Inno Setup**: https://jrsoftware.org/isinfo.php
- **NSIS**: https://nsis.sourceforge.io/

---

## 🐛 Troubleshooting

### Build Errors

**"g++ not found"**
- Add MinGW bin directory to PATH
- Verify: `g++ --version`

**"WebView2 not found"**
- Install WebView2 Runtime
- Check Windows 11 updates

**"node not recognized"**
- Install Node.js
- Restart terminal after installation

### Runtime Errors

**IDE doesn't open files**
- Check file paths in File Explorer config
- Ensure read permissions

**Build button does nothing**
- Open a `.cpp` file first
- Check Build Panel for errors

**AI Assistant not working**
- This is mock implementation
- Connect to OpenAI/Anthropic API for production

---

## 🚀 Advanced Features

### Add Real File System
Enhance `src/webview_host.cpp` to use Windows API:
```cpp
#include <windows.h>
#include <filesystem>

// List directory contents
std::vector<std::string> listDirectory(const std::string& path) {
    namespace fs = std::filesystem;
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(path)) {
        files.push_back(entry.path().string());
    }
    return files;
}
```

### Integrate GDB Debugger
```cpp
// In compiler.cpp
std::string debugCommand = "gdb -q " + executablePath;
system(debugCommand.c_str());
```

### Add Git Integration
```cpp
// Execute git commands
std::string gitStatus = "git status";
executeCommand(gitStatus, outputCallback);
```

---

## 📚 Resources

- **MinGW Documentation**: http://mingw-w64.org/
- **WebView2 API**: https://learn.microsoft.com/microsoft-edge/webview2/
- **Monaco Editor**: https://microsoft.github.io/monaco-editor/
- **React Documentation**: https://react.dev/

---

## ✨ What Makes This Special

1. **No Visual Studio Required** - Pure MinGW compilation
2. **Native Performance** - C++ host with native Windows APIs
3. **Modern UI** - React with Monaco editor
4. **Self-Modifying** - Edit the IDE's own source code
5. **Portable** - Single .exe with embedded webview
6. **Extensible** - Add languages, tools, AI features
7. **Beautiful Design** - Professional GitHub-style interface

---

## 🎓 Learning Path

1. **Beginner**: Use as-is for C++ development
2. **Intermediate**: Customize theme and add file types
3. **Advanced**: Integrate debugger and git
4. **Expert**: Add AI code completion, LSP support

---

## 📄 License

MIT License - See LICENSE file

---

## 🤝 Contributing

This is a complete, working IDE. Feel free to:
- Add features
- Improve UI/UX
- Fix bugs
- Add language support
- Integrate AI models

---

**Built with ❤️ for Windows 11**

*No Visual Studio. No WSL. Just pure Windows development.*
