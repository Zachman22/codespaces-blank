# 🔄 Self-Modifying IDE Guide

## How the Hybrid IDE Can Modify Its Own Source Code

This guide demonstrates the IDE's ability to edit, compile, and rebuild itself - a truly meta development experience!

---

## 🎯 Overview

The Hybrid IDE is unique because it can:
1. **Open its own source files** in the editor
2. **Modify its own code** with syntax highlighting
3. **Rebuild itself** with the integrated compiler
4. **Run the new version** immediately

---

## 🚀 Self-Modification Workflow

### Step 1: Open the IDE's Source Code

1. Launch `HybridIDE.exe`
2. In the File Explorer, navigate to the project directory
3. Open any source file, for example:
   - `src/main.cpp` - Entry point
   - `src/webview_host.cpp` - UI integration
   - `src/compiler.cpp` - Build system
   - `frontend/src/App.tsx` - React UI

### Step 2: Make Changes

**Example: Add a New Feature to the Compiler**

Open `src/compiler.cpp` and modify the `compileSource` function:

```cpp
// Add optimization level option
bool Compiler::compileSource(const std::string& sourcePath, 
                             const std::string& outputPath,
                             int optimizationLevel) {
    std::vector<std::string> args = {
        "-std=c++17",
        "-Wall",
        "-O" + std::to_string(optimizationLevel),  // NEW: Variable optimization
        "-o", outputPath,
        sourcePath
    };
    
    // Rest of the code...
}
```

### Step 3: Save Changes

- Press `Ctrl+S` or click Save
- Modified indicator (•) appears on the tab
- Changes are written to disk

### Step 4: Rebuild the IDE

**Option A: Use the Build Button**
1. Keep the modified `.cpp` file open
2. Click **Build** in the toolbar
3. Watch compilation in the Build Panel
4. New `src/compiler.exe` or intermediate objects created

**Option B: Full Rebuild**
1. Open terminal in IDE (if integrated)
2. Run: `.\build.ps1` or `make clean && make`
3. New `HybridIDE.exe` is created

### Step 5: Test the Changes

1. Close the current IDE instance
2. Run the newly built `HybridIDE.exe`
3. Verify your changes work
4. If issues occur, revert using version control

---

## 🎨 Modifying the User Interface

### Change the Theme Colors

1. Open `frontend/src/App.css`
2. Modify the CSS variables:

```css
:root {
  --bg-primary: #1a1a2e;      /* Darker background */
  --accent-blue: #00d9ff;     /* Neon blue */
  --accent-green: #00ff88;    /* Neon green */
  /* Add your colors */
}
```

3. Rebuild the frontend:
```powershell
cd frontend
npm run build
cd ..
```

4. Relaunch `HybridIDE.exe`
5. See your new theme!

### Add New UI Components

1. Open `frontend/src/components/Toolbar.tsx`
2. Add a new button:

```typescript
<button
  className="toolbar-button"
  onClick={onOptimize}
  title="Optimize Code"
>
  <Zap size={18} />
  <span>Optimize</span>
</button>
```

3. Implement the handler in `App.tsx`:

```typescript
const handleOptimize = () => {
  // Call C++ optimization function
  bridge.optimize(currentTab.path);
};
```

4. Rebuild frontend and restart

---

## 🔧 Adding New Backend Features

### Example: Add File Watching

1. Open `src/webview_host.cpp`
2. Add file watching capability:

```cpp
#include <filesystem>
#include <thread>

class FileWatcher {
private:
    std::string watchPath;
    std::function<void(std::string)> callback;
    bool running = false;
    
public:
    FileWatcher(const std::string& path, 
                std::function<void(std::string)> cb)
        : watchPath(path), callback(cb) {}
    
    void start() {
        running = true;
        std::thread([this]() {
            while (running) {
                checkForChanges();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }).detach();
    }
    
    void checkForChanges() {
        // Check file modifications
        // Call callback when file changes detected
    }
};
```

3. Integrate with WebView2 message handling
4. Rebuild and test live reload!

---

## 📝 Common Self-Modification Scenarios

### 1. Add Support for New Language

**Backend (C++):**
```cpp
// In compiler.cpp
bool Compiler::compileSource(const std::string& sourcePath,
                             const std::string& outputPath) {
    std::string extension = getFileExtension(sourcePath);
    
    if (extension == ".cpp" || extension == ".cc") {
        return compileWithGCC(sourcePath, outputPath);
    } else if (extension == ".rs") {  // NEW: Rust support
        return compileWithRustc(sourcePath, outputPath);
    }
    
    return false;
}

bool Compiler::compileWithRustc(const std::string& src,
                                const std::string& out) {
    std::vector<std::string> args = {"rustc", src, "-o", out};
    return executeCommand("rustc", args);
}
```

**Frontend (TypeScript):**
```typescript
// In FileExplorer.tsx
const langMap: { [key: string]: string } = {
  'cpp': 'cpp',
  'rs': 'rust',  // NEW: Rust syntax highlighting
  'go': 'go',    // NEW: Go support
};
```

### 2. Enhance Build Panel

**Add Build Statistics:**

```cpp
// In compiler.cpp
struct BuildStats {
    int totalFiles;
    int compiledFiles;
    int errors;
    int warnings;
    double buildTime;
};

BuildStats Compiler::getBuildStats() {
    // Calculate and return stats
}
```

**Update UI:**
```typescript
// In BuildPanel.tsx
<div className="build-stats">
  <span>Files: {stats.compiledFiles}/{stats.totalFiles}</span>
  <span>Errors: {stats.errors}</span>
  <span>Warnings: {stats.warnings}</span>
  <span>Time: {stats.buildTime}s</span>
</div>
```

### 3. Add Plugin System

```cpp
// In main.cpp
class PluginManager {
public:
    void loadPlugin(const std::string& pluginPath) {
        HMODULE plugin = LoadLibrary(pluginPath.c_str());
        if (plugin) {
            auto initFunc = (PluginInit)GetProcAddress(plugin, "Initialize");
            if (initFunc) {
                initFunc(getAPIInterface());
            }
        }
    }
};
```

---

## 🧪 Testing Self-Modifications

### Safe Testing Workflow

1. **Backup Current Version**
```powershell
cp HybridIDE.exe HybridIDE_backup.exe
```

2. **Make Changes in IDE**
3. **Build New Version**
```powershell
.\build.ps1 -Output HybridIDE_test.exe
```

4. **Test New Version**
```powershell
.\HybridIDE_test.exe
```

5. **If Successful, Replace**
```powershell
mv HybridIDE_test.exe HybridIDE.exe
```

### Use Version Control

```bash
git init
git add .
git commit -m "Base version"

# Make changes
git diff  # Review changes in IDE
git commit -m "Added feature X"

# Revert if needed
git checkout HEAD -- src/compiler.cpp
```

---

## 🎓 Advanced Self-Modification

### 1. Hot Reload (Without Restart)

Implement dynamic library loading:

```cpp
// Plugin architecture
class DynamicFeature {
public:
    virtual void execute() = 0;
};

// Load new features at runtime
HMODULE feature = LoadLibrary("feature_module.dll");
auto createFunc = (CreateFeature)GetProcAddress(feature, "Create");
DynamicFeature* newFeature = createFunc();
```

### 2. Script-Based Extensions

Add Lua/Python scripting:

```cpp
#include <lua.hpp>

class ScriptEngine {
public:
    void executeScript(const std::string& script) {
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);
        
        // Expose IDE API to Lua
        exposeAPI(L);
        
        luaL_dostring(L, script.c_str());
        lua_close(L);
    }
};
```

### 3. Live UI Updates

Use WebView2's CoreWebView2.Reload():

```cpp
void WebViewHost::reloadUI() {
    if (webview) {
        webview->Reload();
    }
}

// Watch for frontend changes
FileWatcher watcher("frontend/dist", [this](std::string path) {
    reloadUI();
});
```

---

## 🔒 Safety Considerations

### Prevent Corruption

1. **Always keep backups**
2. **Test in separate executable first**
3. **Use version control (Git)**
4. **Validate changes before rebuild**

### Build Verification

```cpp
// Add checksum verification
bool verifyBuild(const std::string& exePath) {
    // Calculate hash of build
    // Compare with expected hash
    // Return true if valid
}
```

### Rollback Mechanism

```cpp
// Automatic backup before rebuild
void backupCurrentVersion() {
    std::filesystem::copy_file(
        "HybridIDE.exe",
        "HybridIDE.exe.backup",
        std::filesystem::copy_options::overwrite_existing
    );
}
```

---

## 🎯 Real-World Use Cases

### 1. **Customize for Your Workflow**
- Add keyboard shortcuts
- Integrate with your tools
- Custom build configurations

### 2. **Learn by Doing**
- Understand IDE architecture
- Experiment with features
- Debug live in the editor

### 3. **Create Domain-Specific IDE**
- Add industry-specific tools
- Custom syntax highlighting
- Specialized compilers

### 4. **Educational Tool**
- Teach IDE development
- Demonstrate compiler construction
- Show GUI programming concepts

---

## 📚 Resources for Self-Modification

### IDE Architecture
- `src/main.cpp` - Application entry point
- `src/webview_host.cpp` - UI and messaging
- `src/compiler.cpp` - Build system
- `src/system_info.cpp` - OS detection
- `frontend/src/App.tsx` - Main UI logic

### Key Technologies
- **WebView2**: UI rendering
- **MinGW**: Compilation
- **React**: Frontend framework
- **Monaco Editor**: Code editing
- **Windows API**: System integration

---

## 🌟 Conclusion

The Hybrid IDE is a **living, breathing development environment** that you can mold to your exact needs. By opening its own source code, making changes, and rebuilding itself, you create a truly personalized tool.

**The IDE is not just a tool - it's a canvas for your development ideas!**

---

## 💡 Challenge Ideas

1. Add dark/light theme toggle
2. Implement project templates
3. Add git integration
4. Create extension marketplace
5. Integrate AI code completion
6. Add collaborative editing
7. Build mobile app companion
8. Create cloud sync feature

**Start modifying and make it yours!** 🚀
