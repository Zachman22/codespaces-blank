# Script Generator Chatbot - Multi-Platform Script Automation

## Overview

The **Script Generator Chatbot** is an intelligent assistant that automatically generates cross-platform scripts for Linux, macOS, and Windows. It features automatic code signing, smart naming, and supports all major operating system versions currently in production.

## Features

### 🎯 Multi-Platform Support

**Linux Distributions:**
- Ubuntu 20.04, 22.04, 24.04
- Debian 10, 11, 12
- Fedora 38, 39, 40
- CentOS 7, 8, 9
- RHEL 7, 8, 9
- Arch Linux
- openSUSE Leap 15.x
- SUSE Linux Enterprise

**macOS Versions:**
- macOS Sonoma (14.x)
- macOS Ventura (13.x)
- macOS Monterey (12.x)
- macOS Big Sur (11.x)
- macOS Catalina (10.15)

**Windows Versions:**
- Windows 11 (22H2, 23H2)
- Windows 10 (21H2, 22H2)
- Windows Server 2022
- Windows Server 2019
- Windows Server 2016

### ✨ Key Features

- ✅ **Automatic Code Signing** - All scripts are signed with SHA256 signatures
- ✅ **Smart Auto-Naming** - Intelligent naming based on script type and timestamp
- ✅ **Cross-Platform Compatible** - Generate for one platform or all at once
- ✅ **Error Handling Built-In** - Comprehensive error detection and handling
- ✅ **Admin Privilege Detection** - Automatic root/admin checks where needed
- ✅ **Package Manager Support** - Auto-detects apt, yum, pacman, brew, choco
- ✅ **Dependency Installation** - Automatically installs required tools
- ✅ **Custom Naming** - User can override auto-generated names

## Quick Start

### Build the Chatbot

```bash
cd hybrid-ide
cmake -S . -B build
cmake --build build --target ScriptGeneratorChatbot
```

### Run the Chatbot

```bash
./build/bin/ScriptGeneratorChatbot
```

Or use the quick-start script:

```bash
./run_script_generator.sh
```

## Usage

### Interactive Mode

```
script-gen> generate
```

The chatbot will guide you through:

1. **Script Description** - What does your script do?
2. **Script Type** - installer, updater, backup, deployment, etc.
3. **Target Platform** - Which OS to generate for (or "all")
4. **Commands** - The actual commands to execute
5. **Dependencies** - Any required packages (optional)
6. **Admin Privileges** - Does it need root/admin?
7. **Auto-Sign** - Sign with CA? (default: yes)
8. **Custom Name** - Override auto-generated name (optional)

### Example Session

```
script-gen> generate

Script Description: Install Docker on server
Script Type: installer
Target Platform: all
Enter commands (one per line, type 'done' when finished):
  > curl -fsSL https://get.docker.com | sh
  > docker --version
  > done
Dependencies: curl
Requires admin privileges? yes
Auto-sign with CA? yes
Custom script name: docker_install

🔄 Generating scripts...
✅ Generated 5 script(s):
  ✓ docker_install.sh [SIGNED] (Linux Bash)
  ✓ docker_install.sh [SIGNED] (macOS Bash)
  ✓ docker_install.zsh [SIGNED] (macOS Zsh)
  ✓ docker_install.ps1 [SIGNED] (Windows PowerShell)
  ✓ docker_install.bat [SIGNED] (Windows Batch)
```

## Script Types

The chatbot supports 10 predefined script types:

1. **installer** - Installation scripts
2. **updater** - Update/upgrade scripts
3. **backup** - Backup and restore scripts
4. **deployment** - Deployment automation
5. **automation** - General automation tasks
6. **sysadmin** - System administration
7. **build** - Build and compilation scripts
8. **test** - Testing scripts
9. **docker** - Docker/container scripts
10. **custom** - Custom scripts

## Supported Platforms

You can generate scripts for specific platforms:

1. **linux_bash** - Linux Bash scripts
2. **linux_sh** - Linux POSIX sh scripts
3. **macos_bash** - macOS Bash scripts
4. **macos_zsh** - macOS Zsh scripts (default on newer macOS)
5. **windows_ps** - Windows PowerShell scripts
6. **windows_cmd** - Windows CMD scripts
7. **windows_batch** - Windows Batch files
8. **all** - Generate for all platforms at once

## Output Directory

**Windows:**
```
G:\Zachary's Playroom Powerd by AI\generated_scripts\
```

**Linux/macOS:**
```
~/ZacharysPlayroom/generated_scripts/
```

All scripts are saved with executable permissions (on Unix-like systems).

## Generated Script Structure

### Linux/macOS Scripts Include:

- ✅ Shebang line (#!/bin/bash or #!/bin/zsh)
- ✅ Header with description and timestamp
- ✅ Error handling (set -e, set -u, set -o pipefail)
- ✅ Root privilege checks (if required)
- ✅ Prerequisite checks
- ✅ Dependency installation (apt/yum/pacman/brew)
- ✅ Main command execution with error checking
- ✅ Success/failure reporting
- ✅ Digital signature footer

### Windows Scripts Include:

- ✅ PowerShell/CMD header
- ✅ Error handling ($ErrorActionPreference = "Stop")
- ✅ Administrator privilege checks
- ✅ Prerequisite checks
- ✅ Dependency installation (Chocolatey)
- ✅ Main command execution with error checking
- ✅ Success/failure reporting
- ✅ Digital signature footer

## Code Signing

All scripts are automatically signed with SHA256 signatures:

```bash
# SIGNATURE: SHA256:94c01c45fb977040
# Generated by Zachary's Playroom Script Generator
```

This ensures:
- Script integrity verification
- Tamper detection
- Authenticity confirmation

## Commands

### Available Commands

- `generate` or `create` - Create a new script
- `platforms` - List all supported platforms
- `help` or `?` - Show detailed help
- `clear` or `cls` - Clear the screen
- `exit` or `quit` - Quit the chatbot

### Platform Information

```
script-gen> platforms
```

Shows all supported Linux distributions, macOS versions, and Windows versions.

## Advanced Features

### Dependency Detection

The generator automatically detects package managers:

- **Linux:** apt-get, yum, pacman, zypper
- **macOS:** brew
- **Windows:** choco

### Error Handling

All generated scripts include:

- Exit on error (fails fast)
- Command status checking
- Detailed error messages
- Proper exit codes (0 = success, 1+ = error)

### Admin Privilege Detection

Scripts automatically check for:

- **Linux/macOS:** Root user (EUID = 0)
- **Windows:** Administrator role

And exit with a clear message if privileges are insufficient.

## Examples

### Example 1: Backup Script

```bash
Description: Daily backup of /var/www
Type: backup
Platform: linux_bash
Commands:
  tar -czf /backup/www-$(date +%Y%m%d).tar.gz /var/www
  find /backup -name "www-*.tar.gz" -mtime +30 -delete
Dependencies: tar
Requires admin: yes
Auto-sign: yes
Custom name: daily_backup
```

### Example 2: System Update Script

```bash
Description: Update all packages
Type: updater
Platform: all
Commands:
  # Linux: apt update && apt upgrade -y
  # macOS: brew update && brew upgrade
  # Windows: choco upgrade all -y
Dependencies: 
Requires admin: yes
Auto-sign: yes
Custom name: system_update
```

### Example 3: Docker Deployment

```bash
Description: Deploy Docker container
Type: deployment
Platform: linux_bash
Commands:
  docker pull myapp:latest
  docker stop myapp || true
  docker rm myapp || true
  docker run -d --name myapp -p 80:80 myapp:latest
Dependencies: docker
Requires admin: no
Auto-sign: yes
Custom name: deploy_myapp
```

## File Naming Convention

Auto-generated names follow this pattern:

```
{type}_{platform}_{timestamp}.{ext}

Examples:
- install_linux_20251210_084042.sh
- backup_macos_20251210_084056.zsh
- deploy_windows_20251210_084112.ps1
```

Custom names are sanitized:
- Spaces converted to underscores
- Special characters removed
- Lowercase conversion
- Platform extension added

## Troubleshooting

### Issue: Permission Denied

**Linux/macOS:**
```bash
chmod +x generated_scripts/*.sh
```

**Windows:**
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### Issue: Command Not Found

Ensure the script's dependencies are installed. The generator includes dependency checks, but you may need to install tools manually:

**Linux:**
```bash
sudo apt-get install -y curl wget git
```

**macOS:**
```bash
brew install curl wget git
```

**Windows:**
```powershell
choco install curl wget git -y
```

### Issue: Output Directory Not Created

The generator automatically creates directories, but you can manually create:

**Linux/macOS:**
```bash
mkdir -p ~/ZacharysPlayroom/generated_scripts
```

**Windows:**
```powershell
New-Item -ItemType Directory -Force -Path "G:\Zachary's Playroom Powerd by AI\generated_scripts"
```

## Integration with Main IDE

The Script Generator Chatbot is part of the Hybrid IDE suite and can be used alongside:

- **AutoCoder Chatbot** - For code generation
- **HybridIDE** - Main IDE application
- **Docker Manager** - Container management
- **AI Agent System** - Intelligent automation

## Technical Details

**Language:** C++ 17  
**Build System:** CMake 3.15+  
**Dependencies:** OpenSSL (for Linux/macOS)  
**Binary Size:** ~400KB  
**Supported Compilers:** GCC 7+, Clang 6+, MSVC 2019+

## Source Files

- `include/script_generator.h` - Header file with class definitions
- `src/script_generator.cpp` - Core implementation (850+ lines)
- `script_generator_main.cpp` - Interactive chatbot interface

## Building from Source

```bash
# Clone repository
git clone https://github.com/Zachman22/codespaces-blank.git
cd codespaces-blank/hybrid-ide

# Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --target ScriptGeneratorChatbot

# Run
./build/bin/ScriptGeneratorChatbot
```

## License

Part of the Hybrid IDE project.  
See LICENSE file for details.

## Support

For issues, questions, or feature requests, please visit:
https://github.com/Zachman22/codespaces-blank

---

**Generated by Zachary's Playroom Powered by AI** 🚀
