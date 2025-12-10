# Hybrid IDE - Developer Documentation

## Architecture Overview

The Hybrid IDE is built with three main components:

1. **C++ Native Host** - Windows application using WinAPI and WebView2
2. **React Frontend** - Modern UI built with React, TypeScript, and Monaco Editor
3. **WebView2 Bridge** - Bidirectional communication layer between C++ and JavaScript

## Project Structure

```
hybrid-ide/
├── src/                      # C++ source files
│   ├── main.cpp             # Application entry point
│   ├── webview_host.cpp     # WebView2 integration
│   ├── compiler.cpp         # MinGW compiler wrapper
│   ├── system_info.cpp      # OS/Architecture detection
│   └── prompt_verifier.cpp  # Build request validation
├── include/                  # C++ headers
├── frontend/                 # React application
│   ├── src/
│   │   ├── App.tsx          # Main application component
│   │   ├── components/      # UI components
│   │   └── services/        # Bridge service
│   └── package.json
├── scripts/                  # Build and packaging scripts
├── build.ps1                # PowerShell build script
├── Makefile                 # Make build configuration
└── README.md
```

## C++ Backend

### Main Components

#### 1. WebView Host (`webview_host.cpp`)

The WebView host manages the WebView2 control and handles message passing:

```cpp
class WebViewHost {
public:
    bool initialize(HWND hwnd, const std::wstring& initialURL);
    void postMessage(const std::string& message);
    void registerMessageHandler(const std::string& type, Handler handler);
};
```

**Key Functions:**
- `initialize()` - Creates WebView2 environment and controller
- `postMessage()` - Sends messages from C++ to JavaScript
- `registerMessageHandler()` - Registers callbacks for JavaScript messages

#### 2. Compiler (`compiler.cpp`)

Wraps MinGW g++ compiler with build management:

```cpp
struct CompileOptions {
    std::string sourceFile;
    std::string outputFile;
    std::vector<std::string> flags;
    // ...
};

class Compiler {
public:
    bool compile(const CompileOptions& options, LogCallback callback);
};
```

**Features:**
- Auto-detection of MinGW installation
- Real-time build log streaming
- Error/warning parsing
- Customizable compilation flags

#### 3. System Info (`system_info.cpp`)

Detects system specifications:

```cpp
class SystemInfo {
public:
    static std::string getOSVersion();      // Windows version
    static std::string getArchitecture();   // x64/ARM64
    static std::string getCPUModel();       // Processor name
    static int getCPUCores();               // Core count
    static unsigned long long getTotalRAM(); // RAM in MB
};
```

#### 4. Prompt Verifier (`prompt_verifier.cpp`)

Validates build requests before execution:

```cpp
struct VerificationResult {
    bool valid;
    std::string message;
    std::vector<std::string> warnings;
};

class PromptVerifier {
public:
    static VerificationResult verifyBuildRequest(
        const std::string& sourceFile,
        const std::string& outputFile
    );
};
```

**Checks:**
- File existence
- Valid source file extensions
- Compiler availability
- Output file validity
- File size warnings

### Message Protocol

Messages between C++ and JavaScript use JSON format:

#### C++ → JavaScript

```json
{
  "type": "buildLog",
  "data": {
    "message": "[INFO] Starting compilation...\n"
  }
}
```

#### JavaScript → C++

```json
{
  "type": "build",
  "data": {
    "sourceFile": "C:/project/main.cpp",
    "outputFile": "C:/project/main.exe",
    "flags": ["-O2", "-Wall"]
  }
}
```

### Message Types

| Type | Direction | Purpose |
|------|-----------|---------|
| `build` | JS→C++ | Request build |
| `buildLog` | C++→JS | Stream build output |
| `buildComplete` | C++→JS | Build finished |
| `run` | JS→C++ | Execute program |
| `runLog` | C++→JS | Program output |
| `runComplete` | C++→JS | Execution finished |
| `getSystemInfo` | JS→C++ | Request system info |
| `systemInfo` | C++→JS | System specifications |

## React Frontend

### Component Structure

```
App.tsx (Main)
├── Toolbar (Build/Run/Debug controls)
├── FileExplorer (Project files)
├── Editor (Monaco code editor)
├── BuildPanel (Console output)
├── AISidebar (AI assistant)
└── SystemInfo (OS/CPU info)
```

### Key Components

#### App.tsx

Main application state and orchestration:

```typescript
interface Tab {
  id: string;
  path: string;
  content: string;
  language: string;
  modified: boolean;
}
```

Manages:
- Open tabs/files
- Build state
- Log output
- UI panel visibility

#### Bridge Service (`services/bridge.ts`)

Handles WebView2 communication:

```typescript
class Bridge {
  send(type: string, data: any): void;
  on(type: string, handler: Handler): void;
  
  // Convenience methods
  build(sourceFile: string, outputFile: string): void;
  run(path: string): void;
  getSystemInfo(): void;
}
```

**Features:**
- Message queuing for early sends
- Type-safe message handlers
- Mock mode for development
- Auto-detection of WebView2 environment

#### Monaco Editor Integration

Uses `@monaco-editor/react` for code editing:

```typescript
<Editor
  language="cpp"
  theme="vs-dark"
  value={content}
  onChange={handleChange}
  options={{
    fontSize: 14,
    minimap: { enabled: true },
    lineNumbers: 'on'
  }}
/>
```

## Build System

### PowerShell Build (`build.ps1`)

Main build script for Windows:

```powershell
.\build.ps1 [-Clean] [-Frontend] [-Backend] [-Release] [-Debug]
```

**Options:**
- `-Clean` - Remove build artifacts
- `-Frontend` - Build only React app
- `-Backend` - Build only C++ host
- `-Release` - Optimize build
- `-Debug` - Include debug symbols (default)

**Steps:**
1. Check for required tools (g++, node, npm)
2. Build React frontend with Vite
3. Compile C++ source files
4. Link executable
5. Copy frontend to output

### Makefile

Alternative build using Make:

```bash
make              # Build all
make clean        # Clean artifacts
make rebuild      # Clean and build
make run          # Build and run
```

### Cross-compilation (Linux)

Build for Windows from Linux:

```bash
chmod +x build.sh
./build.sh
```

Requires MinGW cross-compiler:
```bash
sudo apt-get install mingw-w64
```

## Packaging

Create distributable package:

```powershell
.\scripts\package.ps1 [-OutputName "HybridIDE-Portable"] [-Version "1.0.0"]
```

**Output:**
- Folder with all files
- ZIP archive
- Start.bat launcher
- Configuration file
- Installation guide

## Configuration

Edit `config.json` to customize:

```json
{
  "compiler": {
    "path": "g++",
    "flags": ["-std=c++17", "-Wall"]
  },
  "editor": {
    "fontSize": 14,
    "theme": "vs-dark"
  }
}
```

## Development Workflow

### 1. Setup Development Environment

```powershell
# Install MinGW-w64
# Install Node.js and npm
# Install WebView2 SDK (optional, for advanced features)

# Clone repository
cd hybrid-ide

# Install frontend dependencies
cd frontend
npm install
cd ..
```

### 2. Build

```powershell
# Full build
.\build.ps1

# Frontend only (faster iteration)
.\build.ps1 -Frontend

# Backend only
.\build.ps1 -Backend
```

### 3. Test

```powershell
cd output
.\HybridIDE.exe
```

### 4. Debug

**Frontend:** Use browser dev tools (F12 in WebView2)

**Backend:** Attach debugger to HybridIDE.exe process

### 5. Package

```powershell
.\scripts\package.ps1
```

## Extending the IDE

### Adding a New Message Type

#### 1. C++ Side

```cpp
// In main.cpp
void handleNewFeature(const std::string& data) {
    // Process request
    // Send response
    sendToFrontend("newFeatureResponse", responseData);
}

// Register handler
g_webViewHost->registerMessageHandler("newFeature", handleNewFeature);
```

#### 2. Frontend Side

```typescript
// In bridge.ts
public newFeature(params: any) {
  this.send('newFeature', params);
}

// In component
useEffect(() => {
  bridge.on('newFeatureResponse', (data) => {
    // Handle response
  });
}, []);
```

### Adding a New Component

```typescript
// components/NewComponent.tsx
import React from 'react';

const NewComponent: React.FC<Props> = (props) => {
  return <div>New Component</div>;
};

export default NewComponent;
```

Add to App.tsx:
```typescript
import NewComponent from './components/NewComponent';

// In render
<NewComponent />
```

### Adding Compiler Features

```cpp
// In compiler.cpp
void Compiler::newCompilerFeature() {
    // Implementation
}
```

## Troubleshooting

### Build Errors

**"g++ not found"**
- Install MinGW-w64
- Add bin directory to PATH
- Restart terminal

**"WebView2.h not found"**
- Install WebView2 SDK
- Update paths in build.ps1
- Or build without WebView2 headers (runtime will still work)

**"npm command not found"**
- Install Node.js
- Restart terminal

### Runtime Issues

**IDE won't start**
- Install WebView2 Runtime
- Check Windows version (10/11 required)

**Build button doesn't work**
- Verify MinGW installation: `g++ --version`
- Check PATH environment variable
- Restart IDE after PATH changes

**Files don't appear**
- Implement file system integration in C++
- Currently uses mock file tree

## Performance Optimization

### Frontend

- Use React.memo for expensive components
- Debounce editor changes
- Virtualize file tree for large projects
- Lazy load Monaco Editor

### Backend

- Use async operations for I/O
- Thread pool for builds
- Cache compiler detection results
- Stream build logs efficiently

## Security Considerations

- Validate all file paths (prevent directory traversal)
- Sanitize compiler input
- Limit process execution permissions
- Sandbox compiler output directory

## Future Enhancements

- [ ] GDB debugger integration
- [ ] Multi-file project support
- [ ] CMake integration
- [ ] IntelliSense/code completion
- [ ] Git integration
- [ ] Plugin system
- [ ] Themes/customization
- [ ] Cloud storage integration
- [ ] Remote compilation
- [ ] Cross-platform support (Linux/macOS)

## Contributing

When contributing:
1. Follow existing code style
2. Add tests for new features
3. Update documentation
4. Test on Windows 10 and 11
5. Ensure MinGW compatibility

## License

MIT License - see LICENSE file for details.
