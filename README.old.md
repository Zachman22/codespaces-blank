# ZACH'S HYBRID IDE AUTO-BUILDER

A production-ready hybrid Windows IDE built with C++ (MinGW), React, and WebView2.

## Features

- **Native C++ Host**: MinGW-compatible Windows application
- **React Frontend**: Modern UI with Monaco Editor
- **WebView2 Integration**: Embedded browser for seamless integration
- **MinGW Build System**: Compile C++ projects directly from the IDE
- **Real-time Build Logs**: Stream compiler output to the UI
- **OS/Architecture Detection**: Automatic system detection (Windows 11, x64, CPU, RAM)
- **Prompt Verification**: Validates build requests before compilation
- **File Explorer**: Project file navigation
- **Monaco Editor**: Full-featured code editor with syntax highlighting
- **AI Sidebar**: Integrated AI coding assistant
- **Portable EXE**: No Visual Studio required

## System Requirements

- Windows 10/11 (x64)
- MinGW-w64 GCC compiler
- Node.js 18+ and npm
- WebView2 Runtime (usually pre-installed on Windows 11)

## Quick Start

### Recommended: CMake Build (One Command)

```batch
# Windows - Just run this!
cmake_build.bat

# Or with PowerShell
.\cmake_build.ps1

# Linux/macOS
./cmake_build.sh
```

**That's it!** The executable will be at `build\bin\HybridIDE.exe`

See **[INSTALL.md](INSTALL.md)** for 30-second quick start guide.

### Alternative: Manual Build

```powershell
# Build React frontend
cd frontend
npm install
npm run build

# Build C++ native host
cd ..
.\build.ps1
```

### 3. Run the IDE

```powershell
.\output\HybridIDE.exe
```

## Project Structure

```
hybrid-ide/
├── src/                    # C++ source files
│   ├── main.cpp           # Entry point
│   ├── webview_host.cpp   # WebView2 host
│   ├── compiler.cpp       # MinGW compiler integration
│   ├── system_info.cpp    # OS/Architecture detector
│   └── prompt_verifier.cpp # Request validation
├── include/               # C++ headers
│   ├── webview_host.h
│   ├── compiler.h
│   ├── system_info.h
│   └── prompt_verifier.h
├── frontend/              # React application
│   ├── src/
│   │   ├── App.tsx
│   │   ├── components/
│   │   │   ├── FileExplorer.tsx
│   │   │   ├── Editor.tsx
│   │   │   ├── Toolbar.tsx
│   │   │   ├── BuildPanel.tsx
│   │   │   ├── AISidebar.tsx
│   │   │   └── SystemInfo.tsx
│   │   └── services/
│   │       └── bridge.ts
│   ├── package.json
│   └── vite.config.ts
├── scripts/               # Build scripts
│   ├── build.ps1
│   └── package.ps1
├── Makefile              # MinGW build configuration
└── README.md
```

## Architecture

### C++ Native Host
- Initializes WebView2 environment
- Creates native Windows window
- Handles IPC between C++ and JavaScript
- Executes MinGW compiler (g++)
- Captures build output (stdout/stderr)
- Detects system information

### React Frontend
- Monaco Editor for code editing
- File tree explorer
- Build/Run/Debug toolbar
- Real-time console panel
- AI assistant sidebar
- System info display

### WebView2 Bridge
- Bidirectional message passing
- JSON-based protocol
- Async operation support
- Error handling

## Building from Source

### Prerequisites

1. Install MinGW-w64:
```powershell
# Download from: https://www.mingw-w64.org/
# Or install via MSYS2
pacman -S mingw-w64-x86_64-gcc
```

2. Install WebView2 SDK:
```powershell
# Download from Microsoft or use NuGet
nuget install Microsoft.Web.WebView2
```

3. Install Node.js:
```powershell
# Download from: https://nodejs.org/
```

### Build Steps

1. **Build Frontend**:
```powershell
cd frontend
npm install
npm run build
```

2. **Build Native Host**:
```powershell
# Using PowerShell script
.\build.ps1

# Or using Make
make
```

3. **Package Application**:
```powershell
.\scripts\package.ps1
```

## Usage

### Building a Project

1. Open or create a project folder
2. Add source files via File Explorer
3. Write/edit code in Monaco Editor
4. Click "Build" button in toolbar
5. Monitor build output in Console Panel
6. Output binary will be in project's `build/` folder

### Prompt Verification

The IDE validates build requests:
- Checks for source files
- Verifies compiler availability
- Validates build configuration
- Prevents invalid operations

### System Information

View detected system info:
- OS Version (Windows 11/10)
- Architecture (x64/ARM64)
- CPU Model and Cores
- RAM Size

## Configuration

Edit `config.json` to customize:
- Compiler paths
- Build flags
- Editor settings
- UI theme

## Troubleshooting

### WebView2 Not Found
Install WebView2 Runtime: https://developer.microsoft.com/en-us/microsoft-edge/webview2/

### MinGW Not Found
Add MinGW bin directory to system PATH.

### Build Errors
Check `build.log` for detailed error messages.

## License

MIT License - Free for personal and commercial use.

## Credits

Built with:
- WebView2 by Microsoft
- Monaco Editor by Microsoft
- React by Meta
- MinGW-w64 by the MinGW-w64 Team
