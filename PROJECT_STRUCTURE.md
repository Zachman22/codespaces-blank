# ZACH'S HYBRID IDE - PROJECT STRUCTURE

```
hybrid-ide/
│
├── 📄 README.md                    # Main documentation and overview
├── 📄 DOCUMENTATION.md             # Complete developer documentation  
├── 📄 QUICKSTART.md                # Quick start guide for users
├── 📄 CHANGELOG.md                 # Version history and changes
├── 📄 LICENSE                      # MIT License
├── 📄 .gitignore                   # Git ignore patterns
├── 📄 config.json                  # Runtime configuration
├── 📄 Makefile                     # Make build configuration
├── 📄 build.ps1                    # PowerShell build script (Windows)
├── 📄 build.sh                     # Bash build script (Linux cross-compile)
├── 📄 Start.bat                    # Windows launcher with checks
│
├── 📁 include/                     # C++ Header Files
│   ├── 📄 webview_host.h          # WebView2 host interface
│   ├── 📄 compiler.h               # MinGW compiler wrapper interface
│   ├── 📄 system_info.h            # OS/Architecture detection interface
│   └── 📄 prompt_verifier.h        # Build request validation interface
│
├── 📁 src/                         # C++ Source Files
│   ├── 📄 main.cpp                 # Application entry point + message routing
│   ├── 📄 webview_host.cpp         # WebView2 integration + IPC
│   ├── 📄 compiler.cpp             # MinGW wrapper + build management
│   ├── 📄 system_info.cpp          # System information detection
│   └── 📄 prompt_verifier.cpp      # Request validation logic
│
├── 📁 frontend/                    # React Frontend Application
│   ├── 📄 package.json             # npm dependencies and scripts
│   ├── 📄 tsconfig.json            # TypeScript configuration
│   ├── 📄 tsconfig.node.json       # Node TypeScript config
│   ├── 📄 vite.config.ts           # Vite build configuration
│   ├── 📄 index.html               # HTML entry point
│   │
│   └── 📁 src/                     # React Source Code
│       ├── 📄 main.tsx             # React entry point
│       ├── 📄 App.tsx              # Main application component
│       ├── 📄 App.css              # Application styles
│       ├── 📄 index.css            # Global styles
│       │
│       ├── 📁 components/          # React Components
│       │   ├── 📄 FileExplorer.tsx # Project file browser
│       │   ├── 📄 Editor.tsx       # Monaco editor wrapper
│       │   ├── 📄 Toolbar.tsx      # Build/Run/Debug controls
│       │   ├── 📄 BuildPanel.tsx   # Console output panel
│       │   ├── 📄 AISidebar.tsx    # AI assistant interface
│       │   └── 📄 SystemInfo.tsx   # System info display
│       │
│       └── 📁 services/            # Frontend Services
│           └── 📄 bridge.ts        # WebView2 communication bridge
│
├── 📁 scripts/                     # Build and Deployment Scripts
│   └── 📄 package.ps1              # Create distributable package
│
├── 📁 examples/                    # Example C++ Programs
│   ├── 📄 hello_world.cpp          # Simple hello world
│   └── 📄 test_program.cpp         # Comprehensive test program
│
├── 📁 build/                       # Build Artifacts (generated)
│   └── *.o                         # Object files (gitignored)
│
├── 📁 output/                      # Build Output (generated)
│   ├── 📄 HybridIDE.exe            # Compiled executable
│   └── 📁 frontend/                # Built React app
│       └── 📁 dist/                # Production build
│
└── 📁 package/                     # Distribution Package (generated)
    └── 📁 HybridIDE-Portable-1.0.0/
        ├── 📄 HybridIDE.exe
        ├── 📄 Start.bat
        ├── 📄 config.json
        ├── 📄 README.md
        ├── 📄 INSTALL.md
        └── 📁 frontend/

```

## 📊 File Statistics

### C++ Backend
- **Headers**: 4 files
- **Source Files**: 5 files
- **Total Lines**: ~1,500 lines of C++ code

### React Frontend  
- **Components**: 6 React components
- **Services**: 1 bridge service
- **Config Files**: 5 files
- **Total Lines**: ~1,200 lines of TypeScript/React

### Build System
- **Build Scripts**: 3 (PowerShell, Bash, Make)
- **Package Scripts**: 1
- **Config Files**: 2

### Documentation
- **Total Docs**: 5 markdown files
- **Total Pages**: ~50 pages equivalent

## 🔧 Build Outputs

### Development Build
```
output/
├── HybridIDE.exe              (~2-5 MB)
└── frontend/dist/             (~5-10 MB)
    ├── index.html
    ├── assets/
    │   ├── index-*.js         (React + Monaco)
    │   └── index-*.css        (Styles)
    └── monaco-editor/         (Editor files)
```

### Production Package
```
HybridIDE-Portable-1.0.0.zip   (~15-20 MB total)
├── HybridIDE.exe
├── Start.bat
├── config.json
├── README.md
├── INSTALL.md
└── frontend/dist/
```

## 📦 Dependencies

### Build-Time Dependencies
- **MinGW-w64**: GCC 8.0+ (C++17 support)
- **Node.js**: 18.0+ with npm
- **WebView2 SDK**: (optional, for advanced features)

### Runtime Dependencies
- **Windows 10/11**: x64 architecture
- **WebView2 Runtime**: Usually pre-installed
- **MinGW-w64**: For build functionality

### Frontend npm Packages
```json
{
  "dependencies": [
    "react: ^18.2.0",
    "react-dom: ^18.2.0",
    "@monaco-editor/react: ^4.6.0",
    "monaco-editor: ^0.45.0",
    "lucide-react: ^0.294.0"
  ],
  "devDependencies": [
    "typescript: ^5.2.2",
    "vite: ^5.0.8",
    "@vitejs/plugin-react: ^4.2.1"
  ]
}
```

## 🏗️ Build Process Flow

```
1. Frontend Build (Vite)
   frontend/src/*.tsx → frontend/dist/

2. C++ Compilation (MinGW)
   src/*.cpp → build/*.o

3. Linking
   build/*.o → output/HybridIDE.exe

4. Copy Frontend
   frontend/dist/ → output/frontend/

5. Package (Optional)
   output/ → package/*.zip
```

## 🚀 Key Features Map

### File → Feature Mapping

| File | Features |
|------|----------|
| `main.cpp` | Window creation, message routing, app lifecycle |
| `webview_host.cpp` | WebView2 init, IPC, JavaScript bridge |
| `compiler.cpp` | g++ detection, compilation, log streaming |
| `system_info.cpp` | OS/CPU/RAM detection, WinAPI integration |
| `prompt_verifier.cpp` | Input validation, security checks |
| `App.tsx` | UI orchestration, state management, tabs |
| `bridge.ts` | C++↔JS communication, message protocol |
| `FileExplorer.tsx` | File tree, navigation, file selection |
| `Editor.tsx` | Monaco integration, syntax highlighting |
| `BuildPanel.tsx` | Log display, error highlighting |
| `Toolbar.tsx` | Build/Run buttons, actions |
| `AISidebar.tsx` | AI assistant UI (extensible) |
| `SystemInfo.tsx` | System specs display |

## 📝 Message Protocol

### Messages: JavaScript → C++
```
"build"          - Request compilation
"run"            - Execute compiled program  
"getSystemInfo"  - Request system details
"openFile"       - Open file from disk
"saveFile"       - Save file to disk
```

### Messages: C++ → JavaScript
```
"buildLog"       - Stream build output
"buildComplete"  - Build finished (success/fail)
"runLog"         - Program output
"runComplete"    - Execution finished
"systemInfo"     - System specifications
```

## 🎯 Compilation Targets

### Debug Build
```
Flags: -std=c++17 -Wall -O0 -g
Output: HybridIDE.exe (with debug symbols)
Size: ~5 MB
```

### Release Build
```
Flags: -std=c++17 -Wall -O2
Output: HybridIDE.exe (optimized)
Size: ~2 MB
```

## 📈 Performance Metrics

- **Build Time**: ~5-10 seconds (full rebuild)
- **Startup Time**: <1 second
- **Memory Usage**: ~100-200 MB (with Monaco Editor)
- **Compile Speed**: Depends on MinGW and source size

## ✅ Verification Checklist

After building, verify:
- [ ] HybridIDE.exe exists in output/
- [ ] Frontend files in output/frontend/dist/
- [ ] Start.bat launches IDE
- [ ] System info displays correctly
- [ ] Build button triggers compilation
- [ ] Build logs stream in real-time
- [ ] Run button executes programs
- [ ] Monaco editor loads and highlights code

## 🔐 Security Features

- ✅ Path validation (prevent directory traversal)
- ✅ Compiler command sanitization
- ✅ File existence checks
- ✅ Extension validation
- ✅ Output directory sandboxing

## 📚 Documentation Coverage

- ✅ README.md - Project overview
- ✅ DOCUMENTATION.md - Developer guide
- ✅ QUICKSTART.md - User guide
- ✅ CHANGELOG.md - Version history
- ✅ Code comments - Inline documentation
- ✅ Example programs - Usage demonstrations

## 🎨 UI Components Hierarchy

```
App
├── Toolbar
│   ├── Build Button
│   ├── Run Button
│   ├── Debug Button
│   └── Settings Button
├── Main Container
│   ├── FileExplorer
│   │   └── File Tree
│   ├── Editor Area
│   │   ├── Tabs Bar
│   │   ├── Monaco Editor
│   │   └── Build Panel
│   └── AISidebar (toggleable)
└── Status Bar
    ├── SystemInfo
    └── AI Toggle Button
```

## 🔄 Data Flow

```
User Action (UI)
    ↓
React Component Event
    ↓
Bridge Service (bridge.ts)
    ↓
WebView2 postMessage
    ↓
C++ Message Handler
    ↓
C++ Backend Logic
    ↓
WebView2 postMessage (response)
    ↓
Bridge Service Handler
    ↓
React State Update
    ↓
UI Re-render
```

---

**Total Project Size**: ~50 files, ~3,000 lines of code
**Languages**: C++ (60%), TypeScript/React (35%), Scripts (5%)
**Build System**: PowerShell + Make + Bash
**Architecture**: Native C++ + Embedded WebView2 + React SPA
