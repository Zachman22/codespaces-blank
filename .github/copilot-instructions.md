# Hybrid IDE - AI Coding Agent Instructions

## Project Overview
Hybrid IDE is a C++ desktop IDE with React frontend, featuring AI-powered auto-installation, plugin generation, and self-updating capabilities.

**Architecture**: Native C++ backend (WebView2) communicates with React TypeScript frontend via JSON message bridge.

## Key Components

### Backend (C++)
- **Main**: `src/main.cpp` - Entry point, message routing, WebView initialization
- **Plugin System**: `src/plugin_manager.cpp` - Dynamic plugin loading, auto-generation from AI code
- **Auto Installer**: `src/auto_installer.cpp` - Post-build dependency installation, environment setup
- **Auto Updater**: `src/auto_updater.cpp` - Self-updating system with version management
- **Compiler**: `src/compiler.cpp` - C++ compilation wrapper with real-time output
- **File Ops**: `src/file_operations.cpp` - Safe file I/O with validation

### Frontend (React + TypeScript)
- **App.tsx**: Main application shell with tab management
- **Bridge**: `services/bridge.ts` - Backend communication layer
- **Components**: Editor, FileExplorer, BuildPanel, PluginPanel, UpdatePanel, AutoInstallPanel

## Build System

```bash
# Quick build
./build.sh                    # Linux/macOS
build.bat                     # Windows

# CMake build
mkdir build && cd build
cmake ..
cmake --build .
```

**Post-build**: Auto-installer configures environment, installs npm packages, sets up directories.

## Message Bridge Pattern

### Backend → Frontend
```cpp
sendToFrontend("eventType", "{\"key\":\"value\"}");
```

### Frontend → Backend
```typescript
sendMessage('action', { data: value });
onMessage('response', (data) => { /* handle */ });
```

**Critical**: All messages are JSON strings. Parse carefully in C++.

## Plugin System Workflow

1. **AI generates C++ code** → User pastes into PluginPanel
2. **Frontend sends** `generatePlugin` message with code + name
3. **Backend**:
   - Creates plugin directory in `./plugins/{name}/`
   - Writes `plugin.cpp` and `manifest.json`
   - Compiles to shared library (.so/.dll)
   - Auto-loads plugin dynamically
4. **Plugin Entry**: Exports `extern "C" bool plugin_init()`

## Auto-Installation Flow

When `autoInstall` message received:
1. Detect platform (Linux/Windows/macOS)
2. Check dependencies with verify commands
3. Install missing via package manager (apt/brew/choco)
4. Setup paths: `./bin`, `./lib`, `./plugins`, `./cache`
5. Run `cd frontend && npm install`
6. Generate default `config.json`
7. Report progress via `installProgress` messages

## Update System

- **Check**: Polls GitHub releases API
- **Download**: Curls tarball with progress callbacks
- **Verify**: SHA256 checksum validation
- **Install**: Extracts to current directory (creates backup first)
- **Rollback**: Restores from `./backups/` if needed

## Conventions

### Code Style
- C++: Snake_case for functions, PascalCase for classes
- TypeScript: camelCase, React components PascalCase
- Indent: 4 spaces (C++), 2 spaces (TS/TSX)

### Error Handling
- C++ functions return `bool` for success/failure
- Set callbacks for async progress reporting
- Always validate input in `PromptVerifier` before file ops

### File Paths
- Use absolute paths in C++ APIs
- Forward slashes `/` for cross-platform
- Check `fs::exists()` before operations

## Adding Features

### New Backend Handler
1. Add handler function in `main.cpp`: `handleXRequest(const std::string& data)`
2. Register: `g_webViewHost->registerMessageHandler("action", handleXRequest)`
3. Parse JSON manually (simple string operations or add JSON library)
4. Send response: `sendToFrontend("actionComplete", jsonString)`

### New Frontend Component
1. Create in `frontend/src/components/`
2. Use `sendMessage()` / `onMessage()` from bridge
3. Add button/modal trigger in `App.tsx`
4. Style in component or `App.css`

### New C++ Module
1. Add `.h` to `include/`, `.cpp` to `src/`
2. Update `CMakeLists.txt` SOURCES and HEADERS
3. Initialize in `main.cpp` globals
4. Clean up in WinMain cleanup section

## Dependencies
- **C++**: g++/MSVC (C++17), CMake 3.15+, WebView2 (Windows)
- **Frontend**: Node 18+, npm, Vite
- **Optional**: OpenSSL (code signing), webkit2gtk (Linux)

## Testing Build
```bash
cd hybrid-ide
./build.sh                    # Builds C++ + frontend
./bin/HybridIDE              # Run application
```

## Common Pitfalls
- **JSON escaping**: Escape `"`, `\`, `\n` when embedding strings
- **Memory management**: Delete all `new`'d objects in cleanup
- **WebView paths**: Use `file:///` URLs for local HTML
- **Async operations**: Use `std::thread` + `detach()` for long tasks
- **Platform detection**: `#ifdef _WIN32` for Windows-specific code

## Next Steps for AI
When asked to add features:
1. Determine if backend (C++), frontend (React), or both
2. Follow message bridge pattern for communication
3. Update CMakeLists.txt if adding new C++ files
4. Test auto-install flow after building
5. Consider plugin system if feature can be modular
