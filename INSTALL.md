# 🚀 QUICK START - Install in 30 Seconds

## ⚡ Fastest Installation

### Windows
```batch
cmake_build.bat
```
**That's it!** Double-click `build\bin\HybridIDE.exe` to run.

### Linux/macOS
```bash
chmod +x cmake_build.sh
./cmake_build.sh
./build/bin/HybridIDE
```

---

## 📦 What You Get

After running the build script:
- ✅ **HybridIDE.exe** - Your complete IDE
- ✅ **Frontend UI** - React app bundled inside
- ✅ **Examples** - Sample C++ programs
- ✅ **Automatic code signing** - Self-signed certificates

---

## 🔧 Requirements

**Must Have:**
- CMake 3.15+ → [cmake.org](https://cmake.org/download/)
- C++ compiler → MinGW or Visual Studio
- Node.js 18+ → [nodejs.org](https://nodejs.org/)

**Quick Install All (Windows):**
```powershell
choco install cmake mingw nodejs
```

**Quick Install All (Ubuntu):**
```bash
sudo apt install cmake build-essential nodejs npm
```

---

## 📖 Build Methods

### Method 1: One-Click Scripts (Easiest)

**Windows:**
- Double-click `cmake_build.bat`
- Or run `.\cmake_build.ps1`

**Linux/macOS:**
- Run `./cmake_build.sh`

### Method 2: Manual CMake

```bash
# Build frontend
cd frontend && npm install && npm run build && cd ..

# Build backend
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### Method 3: Old Build Scripts (Still Work)

```batch
BUILD.bat          # Windows batch
build.ps1          # PowerShell
./build.sh         # Bash
make               # GNU Make
```

---

## 🎯 After Building

### Run the IDE
```batch
# Windows
.\build\bin\HybridIDE.exe

# Linux/macOS
./build/bin/HybridIDE
```

### Install System-Wide
```bash
# Linux/macOS
sudo cmake --install build

# Windows (as Admin)
cmake --install build
```

### Create Installer Package
```bash
cd build
cpack                           # Creates installer
```

**Output:**
- Windows: `HybridIDE-1.0.0-win64.exe` (NSIS installer)
- Windows: `HybridIDE-1.0.0-win64.zip` (Portable)
- Linux: `HybridIDE-1.0.0-Linux.deb` (Debian package)
- Linux: `HybridIDE-1.0.0-Linux.rpm` (RedHat package)

---

## 🐛 Common Issues

### "CMake not found"
```bash
# Windows: choco install cmake
# Ubuntu: sudo apt install cmake
# macOS: brew install cmake
```

### "g++ not found"
```bash
# Windows: choco install mingw
# Ubuntu: sudo apt install build-essential
# macOS: xcode-select --install
```

### "node not found"
```bash
# Install Node.js from https://nodejs.org/
```

### "OpenSSL not found" (Warning only)
```bash
# Optional - install OpenSSL for better code signing
# Windows: choco install openssl
# Ubuntu: sudo apt install libssl-dev
# macOS: brew install openssl
```

---

## 📂 Project Structure

```
hybrid-ide/
├── CMakeLists.txt              # CMake configuration
├── cmake_build.bat             # Windows build script
├── cmake_build.ps1             # PowerShell build script
├── cmake_build.sh              # Linux/macOS build script
├── CMAKE_INSTALL.md            # Detailed installation guide
├── src/                        # C++ source files
├── include/                    # C++ headers
├── frontend/                   # React UI
└── build/                      # Build output (created)
    └── bin/
        └── HybridIDE.exe       # Your IDE!
```

---

## ⌨️ Using the IDE

1. **Launch**: Run `HybridIDE.exe`
2. **Browse**: File explorer shows your files
3. **Open**: Click any `.cpp` file
4. **Edit**: Full Monaco editor with syntax highlighting
5. **Save**: Press `Ctrl+S`
6. **Build**: Press `F5` or click Build button
7. **Run**: Press `F6` or click Run button
8. **View Output**: Build Panel shows real-time logs

### Keyboard Shortcuts
- `Ctrl+S` - Save file
- `F5` - Build (compile)
- `F6` - Run executable
- `Ctrl+W` - Close tab
- `Ctrl+Tab` - Switch tabs

---

## ✨ Features

- ✅ **Modern UI** - GitHub-inspired dark theme with glassmorphism
- ✅ **Monaco Editor** - Same engine as VS Code
- ✅ **Real-time Builds** - See compiler output live
- ✅ **Multi-file Editing** - Tabs for multiple files
- ✅ **Code Signing** - Automatic certificate generation
- ✅ **File Operations** - Read, write, create, delete
- ✅ **System Detection** - OS, CPU, RAM info
- ✅ **Portable** - No installation required (after build)

---

## 📚 More Documentation

- **CMAKE_INSTALL.md** - Detailed installation guide
- **CODE_SIGNING.md** - Certificate documentation
- **BUILD_VERIFICATION.md** - Testing guide
- **QUICKSTART.md** - Getting started
- **README.md** - Full documentation

---

## 🎉 That's It!

**You now have a complete, professional C++ IDE!**

Build once, code forever. 🚀

---

**Questions?** Check `CMAKE_INSTALL.md` for detailed troubleshooting.

**Powered by AI** • **Zachary's Playroom** • **© 2024**
