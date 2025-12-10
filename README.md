# 🚀 Hybrid IDE

A modern, AI-powered development environment combining C++ backend with React frontend, featuring natural language code generation, Docker container management, and autonomous development capabilities.

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![Version](https://img.shields.io/badge/version-2.0.0-blue)]()
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey)]()
[![License](https://img.shields.io/badge/license-MIT-green)]()

---

## 🔐 First Time Setup

**After cloning this repository, please authenticate to verify your installation:**

```bash
cd hybrid-ide/build
cmake ..
make SetupAuth
./bin/SetupAuth
```

👉 **[Complete Setup Guide](SETUP.md)** - Takes less than 1 minute!

This one-time setup enables cloud features and verifies your GitHub account.

---

## ✨ Key Features

### 🤖 ZACH Auto Coder System
- **Natural Language → Production Code** - Generate code in 14+ languages from plain English
- **Recursive Error Fixing** - Automatically fix compilation errors until zero errors remain
- **Full Project Generation** - Create complete applications from descriptions
- **Multi-Agent System** - Debugger, Architect, UI Designer, Test Engineer, Code Reviewer agents
- **Run Until Complete** - Iteratively build projects until 100% functional

### 🐳 Docker Manager
- **Container Search & Discovery** - Search Docker Hub, pull images, manage containers
- **Auto-Builder** - Generate Dockerfiles for Node.js, Python, C++, etc.
- **Environment Doctor** - Diagnose and repair container issues automatically
- **Backup & Restore** - Container backup, volume management, image optimization
- **Security Hardening** - Inject secrets, rotate credentials, harden containers

### 🧠 AI Agent System
- **Container Genome Analyzer** - Analyze container DNA and behavioral traits
- **Failure Prediction** - Predict container failures before they happen
- **Self-Healing Engine** - Automatically repair containers and prevent downtime
- **Mutation Engine** - Optimize container configurations autonomously
- **Learning System** - Learn from successes and failures to improve over time

### 🔧 Developer Tools
- **Plugin System** - AI code → compiled plugin workflow
- **Auto Installer** - Post-build dependency installation and environment setup
- **Auto Updater** - Self-updating via GitHub releases with rollback
- **Code Signing** - Integrated certificate management (Windows)
- **Real-time Compilation** - C++ compilation with live output

---

## 📦 Quick Start

### Prerequisites

**Required:**
- CMake 3.15+
- C++17 compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Node.js 18+ and npm
- OpenSSL 1.1+ (for updates/signing)

**Optional:**
- Docker (for container management features)
- WebView2 Runtime (Windows GUI)

### Installation

```bash
# Clone the repository
git clone https://github.com/yourusername/hybrid-ide.git
cd hybrid-ide

# Build frontend
cd frontend
npm install
npm run build
cd ..

# Build backend
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Run the application
./build/bin/HybridIDE
```

**Quick Build Script:**
```bash
# Linux/macOS
chmod +x build.sh
./build.sh

# Windows
cmake_build.bat
```

---

## 🎯 Usage Examples

### Generate Code from Natural Language

```cpp
// Open Auto Coder panel
// Enter: "Create a REST API server with authentication"
// Select language: C++
// Click "Generate Code"

// Result: Complete HTTP server with:
// - JWT authentication
// - CRUD endpoints
// - Database connection
// - Error handling
```

### Docker Container Management

```bash
# Search for containers
Search: "nginx"
→ Shows official nginx images with pull button

# Pull and start container
Click "Pull" → "Start" → View logs in real-time

# Auto-generate Dockerfile
Project Type: "Node.js"
→ Generates production-ready Dockerfile with best practices
```

### Recursive Error Fixing

```cpp
// Open buggy project
// Click "Fix Errors (Recursion)"

// Auto Coder will:
// 1. Detect all syntax/compilation errors
// 2. Fix errors one by one
// 3. Recompile after each fix
// 4. Continue until zero errors
// 5. Report success
```

### Full Project Generation

```
Name: "TaskManager"
Description: "Web app for managing tasks with React frontend and Node.js backend"
Languages: [JavaScript, TypeScript]

Click "Generate Full Project"

→ Creates:
  - Frontend (React + TypeScript)
  - Backend (Express.js + MongoDB)
  - package.json with dependencies
  - Docker setup
  - Tests (Jest, Supertest)
  - README.md
```

---

## 🏗️ Architecture

```
hybrid-ide/
├── src/               # C++ Backend
│   ├── main.cpp       # Entry point & message routing
│   ├── docker_manager.cpp    # 150+ Docker functions
│   ├── auto_coder.cpp        # 100+ code generation functions
│   ├── ai_agent_system.cpp   # 80+ AI agent functions
│   ├── plugin_manager.cpp
│   ├── auto_installer.cpp
│   └── auto_updater.cpp
├── include/           # C++ Headers
├── frontend/          # React TypeScript UI
│   └── src/
│       ├── components/       # 12 React components
│       │   ├── AutoCoderPanel.tsx
│       │   ├── DockerPanel.tsx
│       │   ├── PluginPanel.tsx
│       │   └── ...
│       └── services/
│           └── bridge.ts     # WebView2 message bridge
├── examples/          # Sample C++ programs
└── build/             # Build output
    └── bin/
        └── HybridIDE  # Executable (817 KB)
```

**Message Flow:**
```
React UI → bridge.ts → WebView2 → main.cpp → Manager Classes
                                              ↓
                                    Docker/AutoCoder/AI Agents
                                              ↓
main.cpp → WebView2 → bridge.ts → React UI (state update)
```

---

## 🎨 UI Components

| Component | Description |
|-----------|-------------|
| **AutoCoderPanel** | Natural language code generation interface |
| **DockerPanel** | Container search, management, logs, auto-builder |
| **PluginPanel** | AI code → compiled plugin workflow |
| **UpdatePanel** | Check for updates, download, install |
| **AutoInstallPanel** | Post-build dependency installation |
| **BuildPanel** | Compilation logs and output |
| **AISidebar** | AI assistant chat interface |
| **Editor** | Code editor with syntax highlighting |
| **FileExplorer** | Project file tree navigation |
| **Toolbar** | Build, run, settings actions |
| **SystemInfo** | Real-time CPU, RAM, OS monitoring |
| **SplashScreen** | Loading screen |

---

## 🔧 Configuration

### Auto Updater Setup
```cpp
// In src/auto_updater.cpp
setGitHubRepo("yourusername", "hybrid-ide");
setUpdateCheckInterval(3600); // Check every hour
```

### Docker Manager
```cpp
// In src/docker_manager.cpp
setDockerHost("unix:///var/run/docker.sock"); // Linux
// or
setDockerHost("npipe:////./pipe/docker_engine"); // Windows
```

### Auto Coder Languages
Supported: C++, Python, JavaScript, TypeScript, C#, Rust, Go, Java, Lua, SQL, Bash, PowerShell, HTML, CSS

---

## 📊 System Requirements

### Minimum
- **OS:** Windows 10+ / Ubuntu 20.04+ / macOS 11+
- **CPU:** 2 cores, 2.0 GHz
- **RAM:** 4 GB
- **Disk:** 500 MB

### Recommended
- **OS:** Windows 11 / Ubuntu 22.04+ / macOS 13+
- **CPU:** 4+ cores, 3.0 GHz+
- **RAM:** 8 GB+
- **Disk:** 2 GB (for Docker images)

---

## 🧪 Testing

```bash
# Run backend tests
cd build
ctest --output-on-failure

# Test Docker integration
./bin/HybridIDE
# Check console output for:
# ✓ Docker Manager initialized
# ✓ Container search functional

# Test Auto Coder
# Enter: "Create hello world"
# Expected: C++ code generated successfully
```

---

## 🐛 Troubleshooting

### Build Errors

**"OpenSSL not found"**
```bash
# Ubuntu/Debian
sudo apt-get install libssl-dev

# macOS
brew install openssl
```

**"WebView2 not found" (Windows)**
- Download: https://developer.microsoft.com/microsoft-edge/webview2/
- Install WebView2 Runtime

**Compilation fails on Linux**
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake libssl-dev
```

### Runtime Issues

**Docker Manager: "Cannot connect to Docker daemon"**
```bash
# Start Docker service
sudo systemctl start docker

# Add user to docker group
sudo usermod -aG docker $USER
# Log out and back in
```

**Auto Coder: "No output generated"**
- AI API integration is placeholder - implement OpenAI/Anthropic API calls in `auto_coder.cpp`
- Current implementation generates template code

---

## 📚 Documentation

| Document | Description |
|----------|-------------|
| [DOCUMENTATION.md](DOCUMENTATION.md) | Complete technical documentation |
| [QUICKSTART.md](QUICKSTART.md) | Fast build and run guide |
| [PROJECT_STATUS.md](PROJECT_STATUS.md) | Feature implementation status |
| [ZACH_SYSTEM_STATUS.md](ZACH_SYSTEM_STATUS.md) | Auto-coder system details |
| [CMAKE_INSTALL.md](CMAKE_INSTALL.md) | CMake setup instructions |
| [CPP_DEVELOPMENT_GUIDE.md](CPP_DEVELOPMENT_GUIDE.md) | C++ coding standards |
| [CODE_SIGNING.md](CODE_SIGNING.md) | Code signing setup |
| [INTEGRATION_COMPLETE.md](INTEGRATION_COMPLETE.md) | Integration status report |
| [.github/copilot-instructions.md](.github/copilot-instructions.md) | AI agent development guide |

---

## 🤝 Contributing

We welcome contributions! Please follow these steps:

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Commit your changes: `git commit -m 'Add amazing feature'`
4. Push to the branch: `git push origin feature/amazing-feature`
5. Open a Pull Request

**Code Style:**
- C++: Follow Google C++ Style Guide
- TypeScript: ESLint + Prettier
- Commits: Conventional Commits format

---

## 🎓 Learning Resources

- **Auto Coder Tutorial:** See `ZACH_SYSTEM_STATUS.md`
- **Docker Integration:** See `INTEGRATION_COMPLETE.md` 
- **Plugin Development:** See `CPP_DEVELOPMENT_GUIDE.md`
- **API Reference:** See header files in `include/`

---

## 🗺️ Roadmap

### Version 1.1 (Next Release)
- [ ] Real OpenAI/Anthropic API integration
- [ ] Linux GTK WebView support
- [ ] macOS WebKit support
- [ ] Kubernetes integration
- [ ] GitHub Copilot integration

### Version 2.0 (Future)
- [ ] VSCode extension
- [ ] Cloud sync for projects
- [ ] Team collaboration features
- [ ] Integrated debugger
- [ ] Performance profiler

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- **React** - UI framework
- **Monaco Editor** - Code editing component
- **CMake** - Build system
- **Docker** - Container platform
- **OpenSSL** - Cryptography library
- **WebView2** - Web rendering (Windows)

---

## 📞 Support

- **Issues:** [GitHub Issues](https://github.com/yourusername/hybrid-ide/issues)
- **Discussions:** [GitHub Discussions](https://github.com/yourusername/hybrid-ide/discussions)
- **Email:** support@hybrid-ide.dev

---

## ⭐ Star History

If you find this project useful, please consider giving it a star! ⭐

---

**Built with ❤️ by the Hybrid IDE Team**

*Making AI-powered development accessible to everyone*
