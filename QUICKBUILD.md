# Quick Build Guide - Create HybridIDE.exe

## 🚀 Windows Build (Recommended)

### **Option 1: Double-Click Build**
```
1. Double-click BUILD.bat
2. Wait for compilation
3. Run HybridIDE.exe
```

### **Option 2: PowerShell**
```powershell
# Build everything
.\build.ps1

# Or step by step:
cd frontend
npm install
npm run build
cd ..

g++ -std=c++17 -Iinclude src/*.cpp -o HybridIDE.exe -lole32 -loleaut32 -luuid -mwindows
```

### **Option 3: Command Prompt**
```cmd
BUILD.bat
```

---

## 🔧 Manual Build Steps

### **Step 1: Build Frontend**
```bash
cd frontend
npm install          # Install dependencies (first time only)
npm run build        # Build React app → frontend/dist/
cd ..
```

### **Step 2: Compile C++ Backend**
```bash
g++ -std=c++17 ^
    -Iinclude ^
    src/main.cpp ^
    src/webview_host.cpp ^
    src/compiler.cpp ^
    src/system_info.cpp ^
    src/prompt_verifier.cpp ^
    -o HybridIDE.exe ^
    -lole32 -loleaut32 -luuid -lshell32 ^
    -mwindows
```

### **Step 3: Run**
```bash
.\HybridIDE.exe
```

---

## ✅ What You Get

After building, you'll have:

```
hybrid-ide/
├── HybridIDE.exe          ← Your executable! (run this)
├── frontend/
│   └── dist/              ← React UI files (embedded)
└── [all source files]
```

---

## 🎯 Quick Commands

### **Clean Build**
```powershell
# Remove old build
Remove-Item HybridIDE.exe -ErrorAction SilentlyContinue

# Rebuild
.\build.ps1
```

### **Test Build**
```powershell
# Compile and run immediately
.\build.ps1
.\HybridIDE.exe
```

---

## 📦 Distribution

### **Create Portable Package**
```powershell
# Create folder
mkdir HybridIDE-Portable

# Copy files
Copy-Item HybridIDE.exe HybridIDE-Portable/
Copy-Item -Recurse frontend/dist HybridIDE-Portable/webview
Copy-Item README.md HybridIDE-Portable/
Copy-Item LICENSE HybridIDE-Portable/

# Zip it
Compress-Archive -Path HybridIDE-Portable -DestinationPath HybridIDE-v1.0.0.zip
```

---

## 🐛 Troubleshooting

### **"g++ not found"**
```bash
# Install MinGW
# Add to PATH: C:\mingw64\bin

# Test
g++ --version
```

### **"npm not found"**
```bash
# Install Node.js from nodejs.org
# Restart terminal

# Test
node --version
npm --version
```

### **Build fails with errors**
```bash
# Clean and retry
cd frontend
Remove-Item -Recurse node_modules, dist, package-lock.json
npm install
npm run build
cd ..
.\build.ps1
```

---

## ⚡ Fast Build

```powershell
# One command to rule them all
cd frontend && npm install && npm run build && cd .. && .\build.ps1 && .\HybridIDE.exe
```

---

## 🎉 Success!

When build completes:
- ✅ **HybridIDE.exe** created
- ✅ Size: ~500KB - 1MB
- ✅ Ready to run
- ✅ Portable (no installation needed)

**Just double-click HybridIDE.exe to start!**
