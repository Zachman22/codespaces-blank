# 🚀 Using Hybrid IDE for C++ Development

## Quick Start Guide

---

## ✅ **What You Can Do**

1. **Write C++ Code** - Full syntax highlighting
2. **Build Projects** - One-click compilation with MinGW
3. **Run Programs** - Execute and see output
4. **Debug Errors** - Color-coded build logs
5. **Manage Files** - Multi-tab editing
6. **System Info** - Check your environment

---

## 📝 **Step-by-Step Tutorial**

### **1. Launch the IDE**

```powershell
# First time setup
cd frontend
npm install
npm run build
cd ..

# Build the IDE
.\build.ps1

# Run it!
.\HybridIDE.exe
```

---

### **2. Open a C++ File**

**In the IDE:**
1. Look at the **File Explorer** on the left
2. Click on any `.cpp` file (e.g., `main.cpp`)
3. File opens in the **Editor** with syntax highlighting
4. Edit your code with IntelliSense-like features

---

### **3. Write Your First Program**

**Example: Hello World**

```cpp
#include <iostream>

int main() {
    std::cout << "Hello from Hybrid IDE!" << std::endl;
    return 0;
}
```

**Save the file** (Ctrl+S or click Save button)

---

### **4. Build Your Program**

1. Make sure your `.cpp` file is open and active
2. Click the **🔨 Build** button in the toolbar
3. Watch the **Build Panel** at the bottom:
   - Blue: Info messages
   - Green: Success
   - Red: Errors
   - Orange: Warnings

**Example Build Output:**
```
[CMD] g++ -std=c++17 -Wall main.cpp -o main.exe
[INFO] Compiling main.cpp...
[SUCCESS] Build completed in 1.2s
[INFO] Output: main.exe (45 KB)
```

---

### **5. Run Your Program**

1. After successful build, click **▶️ Run**
2. Output appears in the **Build Panel**
3. See your program's stdout/stderr
4. Check exit code

**Example Output:**
```
[INFO] Running main.exe...
Hello from Hybrid IDE!
[INFO] Process exited with code 0
```

---

## 🎯 **Practical Examples**

### **Example 1: Calculator**

Try this complete example:

```cpp
#include <iostream>

class Calculator {
public:
    double add(double a, double b) { return a + b; }
    double subtract(double a, double b) { return a - b; }
    double multiply(double a, double b) { return a * b; }
    double divide(double a, double b) {
        if (b == 0) {
            std::cerr << "Error: Division by zero!\n";
            return 0;
        }
        return a / b;
    }
};

int main() {
    Calculator calc;
    
    std::cout << "10 + 5 = " << calc.add(10, 5) << std::endl;
    std::cout << "10 - 5 = " << calc.subtract(10, 5) << std::endl;
    std::cout << "10 * 5 = " << calc.multiply(10, 5) << std::endl;
    std::cout << "10 / 5 = " << calc.divide(10, 5) << std::endl;
    
    return 0;
}
```

**How to use:**
1. Copy this code into a new file
2. Click **Build**
3. Click **Run**
4. See the results!

---

### **Example 2: Working with Vectors**

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> numbers = {5, 2, 8, 1, 9, 3};
    
    std::cout << "Original: ";
    for (int n : numbers) std::cout << n << " ";
    std::cout << std::endl;
    
    std::sort(numbers.begin(), numbers.end());
    
    std::cout << "Sorted: ";
    for (int n : numbers) std::cout << n << " ";
    std::cout << std::endl;
    
    return 0;
}
```

---

### **Example 3: File I/O**

```cpp
#include <iostream>
#include <fstream>
#include <string>

int main() {
    // Write to file
    std::ofstream outFile("output.txt");
    outFile << "Hello from C++!" << std::endl;
    outFile << "Written by Hybrid IDE" << std::endl;
    outFile.close();
    
    // Read from file
    std::ifstream inFile("output.txt");
    std::string line;
    
    std::cout << "File contents:" << std::endl;
    while (std::getline(inFile, line)) {
        std::cout << line << std::endl;
    }
    inFile.close();
    
    return 0;
}
```

---

## 🔧 **Common Development Tasks**

### **Multiple Files Project**

**main.cpp:**
```cpp
#include <iostream>
#include "utils.h"

int main() {
    greet("Developer");
    return 0;
}
```

**utils.h:**
```cpp
#ifndef UTILS_H
#define UTILS_H

#include <string>

void greet(const std::string& name);

#endif
```

**utils.cpp:**
```cpp
#include "utils.h"
#include <iostream>

void greet(const std::string& name) {
    std::cout << "Hello, " << name << "!" << std::endl;
}
```

**Build command:**
```bash
g++ -std=c++17 main.cpp utils.cpp -o program.exe
```

---

### **Using C++17 Features**

```cpp
#include <iostream>
#include <optional>
#include <string_view>

std::optional<int> divide(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}

void printName(std::string_view name) {
    std::cout << "Name: " << name << std::endl;
}

int main() {
    auto result = divide(10, 2);
    if (result) {
        std::cout << "Result: " << *result << std::endl;
    }
    
    printName("C++17");
    return 0;
}
```

---

## 🐛 **Debugging Tips**

### **Reading Build Errors**

**Syntax Error:**
```
[ERROR] main.cpp:5:1: error: expected ';' before '}' token
    5 | }
      | ^
```
**Fix:** Add missing semicolon on line 4

**Linker Error:**
```
[ERROR] undefined reference to `someFunction()'
```
**Fix:** Make sure all functions are defined or linked

---

### **Common Issues**

**1. "command not found: g++"**
- Install MinGW
- Add to PATH

**2. "Permission denied"**
- Check file permissions
- Run as administrator if needed

**3. "No such file or directory"**
- Check file paths
- Use absolute paths if needed

---

## 💡 **Pro Tips**

### **1. Use Tabs Effectively**
- Open multiple files in tabs
- Switch between them quickly
- Close unused tabs (✕ button)
- Modified files show a dot (•)

### **2. Monitor Build Output**
- Keep Build Panel open
- Check for warnings (orange)
- Fix errors (red) first
- Celebrate success (green)

### **3. Organize Your Code**
- Use the File Explorer
- Group related files
- Follow C++ conventions
- Comment your code

### **4. Leverage System Info**
- Click status bar
- Check your OS/Architecture
- Verify system resources
- Ensure compatibility

---

## 🎨 **Customizing for C++ Development**

### **Add Compiler Flags**

Edit `config.json`:
```json
{
  "compiler": {
    "executable": "g++",
    "flags": [
      "-std=c++17",
      "-Wall",
      "-Wextra",
      "-O2",
      "-g"
    ]
  }
}
```

### **Add More File Types**

For headers, add to `FileExplorer.tsx`:
```typescript
const langMap = {
  'cpp': 'cpp',
  'hpp': 'cpp',
  'h': 'cpp',
  'cc': 'cpp',
  'cxx': 'cpp'
};
```

---

## 📚 **Sample Projects You Can Build**

### **1. Command-Line Tools**
- File manager
- Text editor
- System monitor
- Data processor

### **2. Games**
- Text-based RPG
- Tic-tac-toe
- Snake
- Sudoku solver

### **3. Utilities**
- Password generator
- File encryptor
- JSON parser
- HTTP client

### **4. Data Structures**
- Custom linked list
- Binary tree
- Hash table
- Graph algorithms

---

## 🚀 **Workflow Example**

**Complete Development Session:**

```
1. Launch HybridIDE.exe
   → Splash screen appears
   → IDE loads

2. Open existing file or create new
   → Click in File Explorer
   → Or use File → New

3. Write your C++ code
   → Syntax highlighting active
   → Auto-indent working
   → Line numbers visible

4. Save your work
   → Ctrl+S or Save button
   → Tab shows no dot (saved)

5. Build the project
   → Click Build button
   → Watch Build Panel
   → Check for errors

6. Fix any errors
   → Click on error line
   → Make corrections
   → Rebuild

7. Run the program
   → Click Run button
   → See output in Build Panel
   → Verify results

8. Test and iterate
   → Make changes
   → Rebuild
   → Test again

9. Open multiple files
   → Use tabs
   → Edit simultaneously
   → Switch as needed

10. Use AI Assistant (optional)
    → Click "AI Assistant"
    → Ask coding questions
    → Get help
```

---

## 🎓 **Learning Resources**

### **C++ Basics**
- Variables and types
- Control flow (if, loops)
- Functions
- Classes and objects

### **STL Containers**
- `vector`, `list`, `map`
- `set`, `queue`, `stack`
- Iterators
- Algorithms

### **Modern C++**
- Smart pointers
- Lambda expressions
- Move semantics
- Range-based loops

### **Advanced Topics**
- Templates
- RAII
- Multithreading
- Exception handling

---

## 💻 **Test Your Setup**

**Quick Test Program:**

```cpp
#include <iostream>

int main() {
    std::cout << "✅ Hybrid IDE is working!" << std::endl;
    std::cout << "✅ Compiler is configured!" << std::endl;
    std::cout << "✅ Ready for C++ development!" << std::endl;
    return 0;
}
```

**Expected Output:**
```
✅ Hybrid IDE is working!
✅ Compiler is configured!
✅ Ready for C++ development!
```

---

## 🎯 **Next Steps**

1. **Try the examples** in `/examples` folder
2. **Create your own project**
3. **Experiment with features**
4. **Customize the IDE**
5. **Build something amazing!**

---

## 📞 **Getting Help**

- Check build output for errors
- Read compiler messages carefully
- Verify file paths
- Check MinGW installation
- Review documentation

---

**🎉 You're ready to develop C++ applications with Hybrid IDE!**

**Happy Coding! 🚀**
