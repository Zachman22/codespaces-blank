# Hybrid IDE VS Code Extension

Complete AI-powered VS Code extension that brings executable building, GUI generation, and intelligent chatbot assistance to your IDE.

## Features

### 🤖 AI Chatbot
- Interactive AI assistant integrated directly in VS Code
- Context-aware responses based on your workspace
- Code generation, debugging help, and explanations
- Execute generated code directly from the chat

### 📦 Executable Builder
- Build executables from any source file with one click
- Multi-language support: C++, C, Python, Go, Rust, Java, TypeScript
- Cross-platform builds (Windows, Linux, macOS)
- Automatic dependency detection
- Release and debug builds

### 🎨 GUI Generator
- Automatically generate GUIs from your code
- Analyzes functions and creates interactive interfaces
- Multiple framework support:
  - HTML/CSS/JS - Simple web-based GUI
  - React - Modern component-based UI
  - GTK - Native Linux applications
  - Qt - Cross-platform desktop apps
- Instant preview and customization

### ✨ AI Code Generation
- Generate code from natural language descriptions
- Context-aware suggestions
- Multiple language support
- Insert directly into your editor

## Installation

### From VSIX
1. Download the `.vsix` file
2. Open VS Code
3. Go to Extensions view (Ctrl+Shift+X)
4. Click "..." menu → "Install from VSIX"
5. Select the downloaded file

### From Source
```bash
cd vscode-extension
npm install
npm run compile
npm run package
code --install-extension hybrid-ide-vscode-2.0.0.vsix
```

## Usage

### Open AI Chatbot
- Click the robot icon in the status bar (bottom-right)
- Or: Ctrl+Shift+P → "Hybrid IDE: Open AI Chatbot"
- Ask questions, request code, get debugging help

### Build Executable
- Open any source file (`.cpp`, `.py`, `.go`, etc.)
- Ctrl+Shift+P → "Hybrid IDE: Build Executable"
- Select platform and build type
- Enter output name
- Executable is built in `build/bin/`

### Generate GUI
- Open any source file with functions
- Ctrl+Shift+P → "Hybrid IDE: Generate GUI from Imports"
- Select framework (HTML, React, GTK, Qt)
- GUI is generated in `generated-gui/` folder
- Preview opens automatically

### Generate Code
- Ctrl+Shift+P → "Hybrid IDE: Generate Code with AI"
- Describe what you want to create
- Code is inserted at cursor position

## Configuration

Add to your VS Code `settings.json`:

```json
{
  "hybrid-ide.aiProvider": "openai",
  "hybrid-ide.apiKey": "your-api-key-here",
  "hybrid-ide.model": "gpt-4-turbo-preview"
}
```

### Supported AI Providers
- **OpenAI**: GPT-4, GPT-4 Turbo, GPT-3.5
- **Anthropic**: Claude 3 Opus, Claude 3.5 Sonnet

## Requirements

### For Building Executables
- **C/C++**: GCC or Clang compiler
- **Python**: Python 3.7+, PyInstaller (`pip install pyinstaller`)
- **Go**: Go 1.16+ toolchain
- **Rust**: Rust toolchain with Cargo
- **Java**: JDK 11+ with `javac` and `jar`

### For GUI Generation
- **HTML/CSS/JS**: No requirements (runs in browser)
- **React**: Node.js 16+, npm
- **GTK**: GTK3 development libraries
- **Qt**: Qt5 or Qt6 SDK

## Commands

| Command | Description | Shortcut |
|---------|-------------|----------|
| `hybrid-ide.openChatbot` | Open AI Chatbot | Status bar icon |
| `hybrid-ide.buildExecutable` | Build executable from current file | - |
| `hybrid-ide.generateGUI` | Generate GUI from code | - |
| `hybrid-ide.generateCode` | Generate code with AI | - |

## Extension Architecture

```
vscode-extension/
├── src/
│   ├── extension-simple.ts       # Main extension entry
│   ├── chatbotPanel.ts           # AI chatbot webview
│   ├── executableBuilder.ts      # Build system
│   └── guiGenerator.ts           # GUI generation engine
├── package.json                   # Extension manifest
├── tsconfig.json                  # TypeScript config
└── README.md                      # This file
```

## Examples

### Building a C++ Executable
1. Open `main.cpp`
2. Run "Build Executable" command
3. Enter name: `myapp`
4. Result: `build/bin/myapp` (or `myapp.exe` on Windows)

### Generating a GUI
1. Open Python file with functions:
```python
def calculate_sum(a, b):
    return a + b

def get_username():
    return "User"
```
2. Run "Generate GUI" command
3. Select "HTML/CSS/JS"
4. Result: Interactive HTML form with buttons for each function

### Using the Chatbot
1. Click robot icon in status bar
2. Type: "Create a REST API endpoint for user login"
3. Get instant code generation with explanation
4. Click "Execute" to test

## Known Issues

- Build times may vary based on project size
- GUI generation works best with well-documented functions
- AI responses require valid API keys

## Roadmap

- [ ] Integrated debugger with AI assistance
- [ ] Performance profiler
- [ ] Cloud project sync
- [ ] Team collaboration features
- [ ] Kubernetes deployment
- [ ] Code review AI

## Contributing

This extension is part of the Hybrid IDE project. Contributions welcome!

## License

MIT License - See LICENSE file for details

## Support

- GitHub: https://github.com/Zachman22/codespaces-blank
- Issues: Report bugs and request features on GitHub

---

**Made with ❤️ by Zachary's Playroom**
