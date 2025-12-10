# Hybrid IDE - Complete AI-Powered Development Environment

<div align="center">

![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)

**Next-Generation IDE with AI Integration, Kubernetes Support, and Real-Time Collaboration**

[Features](#features) • [Installation](#installation) • [Quick Start](#quick-start) • [Documentation](#documentation) • [Contributing](#contributing)

</div>

---

## 🚀 Overview

Hybrid IDE is a comprehensive, production-ready development environment that combines the power of modern AI assistants (OpenAI GPT-4, Anthropic Claude) with professional development tools including integrated debugging, performance profiling, Kubernetes deployment, and real-time collaboration.

### Key Highlights

- **🤖 AI Integration**: Native OpenAI GPT-4 and Anthropic Claude 3.5 support
- **🐛 Integrated Debugger**: GDB/LLDB wrapper with modern interface
- **⚡ Performance Profiler**: CPU, memory, and I/O profiling with optimization suggestions
- **☸️ Kubernetes Manager**: Deploy and manage containers directly from IDE
- **👥 Real-Time Collaboration**: Live editing and chat with team members
- **☁️ Cloud Sync**: Automatic project synchronization (AWS, Azure, GCP, GitHub)
- **📜 Script Generator**: Multi-platform script generation (Linux, macOS, Windows)
- **🔌 VSCode Extension**: Bring all features to Visual Studio Code

---

## ✨ Features

### 1. AI-Powered Code Generation

```cpp
// AI API Client supports multiple providers
AIAPIClient client;
client.initialize();

// Generate code from natural language
string code = client.codeCompletion(
    "Create a REST API with authentication",
    "typescript"
);

// Get code review
string review = client.codeReview(code, "typescript");

// Debug assistance
string fix = client.debugAssistance(errorMessage, code);
```

**Supported Models:**
- OpenAI: GPT-4 Turbo, GPT-4, GPT-3.5 Turbo
- Anthropic: Claude 3 Opus, Claude 3.5 Sonnet, Claude 3 Sonnet, Claude 3 Haiku

### 2. Cross-Platform WebView Support

**Linux GTK:**
```cpp
WebViewGTK webview;
webview.create(1280, 720, "Hybrid IDE");
webview.loadURL("http://localhost:3000");
webview.run();
```

**macOS WebKit:**
```cpp
WebViewMacOS webview;
webview.create(1280, 720, "Hybrid IDE");
webview.setTitlebarAppearsTransparent(true);
webview.enableVibrancy(true);
webview.loadURL("http://localhost:3000");
webview.run();
```

**Windows WebView2:** (Already implemented in main codebase)

### 3. Integrated Debugger

```cpp
Debugger debugger;
debugger.initialize(Debugger::DebuggerType::AUTO);
debugger.loadExecutable("./myapp", {"--debug"});

// Set breakpoints
debugger.setBreakpoint("main.cpp", 42);
debugger.setConditionalBreakpoint("processData", "count > 100");

// Execution control
debugger.run();
debugger.stepOver();
debugger.stepInto();

// Inspect variables
auto stackTrace = debugger.getStackTrace();
auto locals = debugger.getLocalVariables();
```

### 4. Performance Profiler

```cpp
Profiler profiler;
profiler.startProfiling("./myapp", Profiler::ProfileType::ALL);

// Run application...

profiler.stopProfiling();

// Analyze results
auto cpuProfile = profiler.getCPUProfile();
auto hotSpots = profiler.getHotSpots(10);
auto suggestions = profiler.getOptimizationSuggestions();

// Export results
profiler.exportFlameGraph("profile.svg");
profiler.exportToJSON("profile.json");
```

### 5. Kubernetes Integration

```cpp
KubernetesManager k8s;
k8s.initialize("~/.kube/config");
k8s.connectToCluster("production");

// Deploy application
k8s.applyYAML("deployment.yaml");

// Manage pods
auto pods = k8s.listPods("default");
string logs = k8s.getPodLogs("myapp-pod-123", "default");

// Scale deployment
k8s.scaleDeployment("myapp", 5, "default");

// Port forwarding
k8s.startPortForward("myapp-pod-123", 8080, 80);
```

### 6. Real-Time Collaboration

```cpp
Collaboration collab;
collab.connect("wss://collab.hybrid-ide.io", token);

// Create/join room
auto room = collab.createRoom("team-project", projectId, true);
collab.joinRoom(room.id);

// Send edits
Collaboration::EditOperation edit;
edit.file = "main.cpp";
edit.action = "insert";
edit.startLine = 10;
edit.content = "cout << \"Hello\" << endl;";
collab.sendEdit(edit);

// Chat
collab.sendMessage("Check out line 42!");
collab.sendCodeSnippet(code, "cpp");
```

### 7. Cloud Sync

```cpp
CloudSync sync;
CloudSync::SyncConfig config;
config.provider = CloudSync::Provider::AWS_S3;
config.bucket = "my-projects";
config.accessKey = getenv("AWS_ACCESS_KEY");
config.secretKey = getenv("AWS_SECRET_KEY");
config.autoSync = true;
config.syncIntervalSeconds = 300;

sync.initialize(config);

// Sync project
auto result = sync.fullSync("/path/to/project");
if (result.status == CloudSync::SyncStatus::SUCCESS) {
    cout << "Synced " << result.filesUploaded << " files" << endl;
}
```

### 8. Multi-Platform Script Generator

```cpp
ScriptGenerator generator;
generator.initialize();

ScriptGenerator::ScriptRequest request;
request.description = "Install Docker and configure firewall";
request.type = ScriptGenerator::ScriptType::INSTALLER;
request.platform = ScriptGenerator::Platform::ALL_PLATFORMS;
request.commands = {
    "curl -fsSL https://get.docker.com | sh",
    "systemctl enable docker",
    "ufw allow 2376/tcp"
};
request.requiresAdmin = true;
request.autoSign = true;

auto scripts = generator.generateScripts(request);
// Generates: docker_install.sh, docker_install.ps1, docker_install.bat, etc.
```

### 9. VSCode Extension

Install the Hybrid IDE extension in VSCode:

```bash
cd vscode-extension
npm install
npm run compile
npm run package
code --install-extension hybrid-ide-vscode-2.0.0.vsix
```

**Features in VSCode:**
- `Ctrl+Shift+G`: Generate code with AI
- `Ctrl+Shift+R`: Review code with AI
- `Ctrl+Shift+D`: Debug with AI assistance
- Kubernetes view and deployment
- Real-time collaboration panel
- Performance profiler view
- Cloud sync status

---

## 📦 Installation

### Prerequisites

**Linux:**
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake libgtk-3-dev libwebkit2gtk-4.0-dev \
    libssl-dev libcurl4-openssl-dev libjsoncpp-dev

# Fedora
sudo dnf install gcc-c++ cmake gtk3-devel webkit2gtk3-devel openssl-devel \
    libcurl-devel jsoncpp-devel

# Arch
sudo pacman -S base-devel cmake gtk3 webkit2gtk openssl curl jsoncpp
```

**macOS:**
```bash
brew install cmake openssl curl jsoncpp
xcode-select --install
```

**Windows:**
```powershell
# Install Visual Studio 2022 with C++ workload
# Install CMake from https://cmake.org/download/
# Install vcpkg for dependencies
vcpkg install curl openssl jsoncpp
```

### Build from Source

```bash
git clone https://github.com/Zachman22/hybrid-ide.git
cd hybrid-ide

# Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build all components
cmake --build build --config Release

# Build specific components
cmake --build build --target HybridIDE
cmake --build build --target AutoCoderChatbot
cmake --build build --target ScriptGeneratorChatbot
```

### Install

```bash
# System-wide installation
sudo cmake --install build

# Or run from build directory
./build/bin/HybridIDE
```

---

## 🎯 Quick Start

### 1. Configure API Keys

Create `~/.hybrid-ide/config.json`:

```json
{
  "openai_api_key": "sk-...",
  "anthropic_api_key": "sk-ant-...",
  "kubernetes": {
    "kubeconfig": "~/.kube/config",
    "default_namespace": "default"
  },
  "cloud_sync": {
    "provider": "aws",
    "bucket": "my-projects",
    "auto_sync": true
  },
  "collaboration": {
    "server_url": "wss://collab.hybrid-ide.io",
    "username": "your-username"
  }
}
```

### 2. Launch Hybrid IDE

```bash
# Main IDE
./build/bin/HybridIDE

# Auto Coder Chatbot
./build/bin/AutoCoderChatbot

# Script Generator
./build/bin/ScriptGeneratorChatbot
```

### 3. Generate Your First Code

```bash
$ ./build/bin/AutoCoderChatbot
> generate a REST API with Express.js and JWT authentication

[AI generates complete Express.js server code]
Code saved to: ~/ZacharysPlayroom/generated_code/express_api_20251210_090000.js
```

### 4. Deploy to Kubernetes

```bash
# From IDE or command line
kubectl config use-context production
./build/bin/HybridIDE --deploy myapp --namespace production
```

---

## 🏗️ Architecture

```
hybrid-ide/
├── include/                     # C++ Headers
│   ├── ai_api_client.h         # OpenAI/Anthropic integration
│   ├── debugger.h              # GDB/LLDB wrapper
│   ├── profiler.h              # Performance profiling
│   ├── kubernetes_manager.h    # K8s deployment
│   ├── collaboration.h         # Real-time collab
│   ├── cloud_sync.h            # Cloud storage sync
│   ├── webview_gtk.h           # Linux WebView
│   ├── webview_macos.h         # macOS WebView
│   └── script_generator.h      # Multi-platform scripts
├── src/                         # C++ Implementation
│   ├── ai_api_client.cpp
│   ├── main.cpp
│   └── ...
├── frontend/                    # React TypeScript UI
│   ├── src/
│   │   ├── components/
│   │   ├── services/
│   │   └── App.tsx
│   └── package.json
├── vscode-extension/            # VSCode Extension
│   ├── src/
│   │   ├── extension.ts
│   │   ├── providers/
│   │   └── services/
│   └── package.json
├── examples/                    # Example projects
├── docs/                        # Documentation
└── CMakeLists.txt              # Build configuration
```

---

## 📚 Documentation

### Core Components

- [AI API Client](docs/ai-api-client.md) - OpenAI and Anthropic integration
- [Debugger](docs/debugger.md) - Integrated debugging with GDB/LLDB
- [Profiler](docs/profiler.md) - Performance analysis and optimization
- [Kubernetes Manager](docs/kubernetes.md) - Container orchestration
- [Collaboration](docs/collaboration.md) - Real-time team features
- [Cloud Sync](docs/cloud-sync.md) - Project synchronization
- [Script Generator](docs/script-generator.md) - Cross-platform scripts

### Platform-Specific

- [Linux GTK WebView](docs/webview-linux.md)
- [macOS WebKit](docs/webview-macos.md)
- [Windows WebView2](docs/webview-windows.md)

### Development

- [Building from Source](docs/building.md)
- [Contributing Guide](CONTRIBUTING.md)
- [API Reference](docs/api-reference.md)
- [Plugin Development](docs/plugins.md)

---

## 🎮 Usage Examples

### Example 1: AI-Assisted Development

```cpp
#include "ai_api_client.h"

int main() {
    AIAPIClient ai;
    ai.loadAPIKeysFromEnvironment();
    
    // Generate boilerplate
    string apiCode = ai.codeCompletion(
        "Create Express API with routes for users CRUD",
        "javascript"
    );
    
    // Review generated code
    string review = ai.codeReview(apiCode, "javascript");
    
    // Optimize
    string optimized = ai.optimizeSuggestions(apiCode, "javascript");
    
    return 0;
}
```

### Example 2: Full Deployment Pipeline

```cpp
#include "kubernetes_manager.h"
#include "cloud_sync.h"
#include "profiler.h"

int main() {
    // 1. Profile application
    Profiler profiler;
    profiler.startProfiling("./myapp");
    // ... run tests ...
    profiler.stopProfiling();
    profiler.exportToJSON("profile.json");
    
    // 2. Sync to cloud
    CloudSync sync;
    sync.initialize(config);
    sync.syncUp("./myapp");
    
    // 3. Deploy to Kubernetes
    KubernetesManager k8s;
    k8s.initialize();
    k8s.applyYAML("k8s/deployment.yaml");
    k8s.scaleDeployment("myapp", 3);
    
    return 0;
}
```

### Example 3: Collaborative Development

```cpp
#include "collaboration.h"

int main() {
    Collaboration collab;
    collab.connect("wss://collab.hybrid-ide.io", token);
    
    auto room = collab.joinRoom("team-project-123");
    
    // Listen for edits
    collab.setEditReceivedCallback([](const auto& edit) {
        cout << "Edit from " << edit.userId << endl;
        // Apply edit to local file
    });
    
    // Send your edits
    Collaboration::EditOperation edit;
    edit.file = "main.cpp";
    edit.action = "insert";
    edit.content = "// TODO: Implement feature X\n";
    collab.sendEdit(edit);
    
    collab.run(); // Event loop
    return 0;
}
```

---

## 🔧 Configuration

### Environment Variables

```bash
export OPENAI_API_KEY="sk-..."
export ANTHROPIC_API_KEY="sk-ant-..."
export KUBECONFIG="~/.kube/config"
export AWS_ACCESS_KEY_ID="..."
export AWS_SECRET_ACCESS_KEY="..."
```

### Config File (`~/.hybrid-ide/config.json`)

```json
{
  "ai": {
    "default_model": "gpt-4-turbo",
    "temperature": 0.7,
    "max_tokens": 2048
  },
  "editor": {
    "theme": "dark",
    "font_size": 14,
    "auto_save": true
  },
  "debugger": {
    "type": "auto",
    "break_on_exception": true
  },
  "profiler": {
    "sampling_interval": 10,
    "stack_depth": 64
  },
  "kubernetes": {
    "context": "production",
    "namespace": "default"
  }
}
```

---

## 🚢 Deployment

### Docker

```dockerfile
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y \
    build-essential cmake libgtk-3-dev libwebkit2gtk-4.0-dev
COPY . /app
WORKDIR /app
RUN cmake -B build && cmake --build build
CMD ["./build/bin/HybridIDE"]
```

### Kubernetes

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: hybrid-ide
spec:
  replicas: 3
  selector:
    matchLabels:
      app: hybrid-ide
  template:
    metadata:
      labels:
        app: hybrid-ide
    spec:
      containers:
      - name: hybrid-ide
        image: zacharysplayroom/hybrid-ide:2.0.0
        ports:
        - containerPort: 3000
        env:
        - name: OPENAI_API_KEY
          valueFrom:
            secretKeyRef:
              name: api-keys
              key: openai
```

---

## 🤝 Contributing

We welcome contributions! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Setup

```bash
git clone https://github.com/Zachman22/hybrid-ide.git
cd hybrid-ide
git checkout -b feature/my-feature

# Make changes
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/bin/HybridIDE

# Submit PR
git push origin feature/my-feature
```

---

## 📝 License

MIT License - see [LICENSE](LICENSE) file for details.

---

## 🌟 Roadmap

### Version 2.0 (Current)
- ✅ OpenAI/Anthropic API integration
- ✅ Linux GTK WebView support
- ✅ macOS WebKit support
- ✅ Kubernetes integration
- ✅ Integrated debugger
- ✅ Performance profiler
- ✅ VSCode extension
- ✅ Cloud sync
- ✅ Real-time collaboration

### Version 2.1 (Q1 2026)
- [ ] GitHub Copilot integration
- [ ] Language Server Protocol (LSP) support
- [ ] Plugin marketplace
- [ ] Mobile companion app
- [ ] AI model fine-tuning

### Version 3.0 (Q3 2026)
- [ ] Web-based IDE (browser version)
- [ ] Multi-language support (Python, Java, Go, Rust)
- [ ] Advanced AI agents for autonomous coding
- [ ] Blockchain integration for code verification

---

## 📞 Support

- **Documentation**: https://hybrid-ide.io/docs
- **Issues**: https://github.com/Zachman22/hybrid-ide/issues
- **Discussions**: https://github.com/Zachman22/hybrid-ide/discussions
- **Email**: support@hybrid-ide.io
- **Discord**: https://discord.gg/hybrid-ide

---

## 🙏 Acknowledgments

- OpenAI for GPT-4 API
- Anthropic for Claude API
- WebKitGTK team
- VSCode extension API
- Kubernetes community
- All contributors and users

---

<div align="center">

**Built with ❤️ by Zachary's Playroom**

⭐ Star us on GitHub | 🐦 Follow on Twitter | 💼 Connect on LinkedIn

</div>
