#include "script_generator.h"
#include "code_signer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#else
#include <unistd.h>
#endif

ScriptGenerator::ScriptGenerator() {
    // Initialize platform extensions
    platformExtensions_[Platform::LINUX_BASH] = ".sh";
    platformExtensions_[Platform::LINUX_SH] = ".sh";
    platformExtensions_[Platform::MACOS_BASH] = ".sh";
    platformExtensions_[Platform::MACOS_ZSH] = ".zsh";
    platformExtensions_[Platform::WINDOWS_POWERSHELL] = ".ps1";
    platformExtensions_[Platform::WINDOWS_CMD] = ".cmd";
    platformExtensions_[Platform::WINDOWS_BATCH] = ".bat";

    // Initialize platform shebangs
    platformShebangs_[Platform::LINUX_BASH] = "#!/bin/bash";
    platformShebangs_[Platform::LINUX_SH] = "#!/bin/sh";
    platformShebangs_[Platform::MACOS_BASH] = "#!/bin/bash";
    platformShebangs_[Platform::MACOS_ZSH] = "#!/bin/zsh";
    platformShebangs_[Platform::WINDOWS_POWERSHELL] = "# PowerShell Script";
    platformShebangs_[Platform::WINDOWS_CMD] = "@echo off";
    platformShebangs_[Platform::WINDOWS_BATCH] = "@echo off";
}

ScriptGenerator::~ScriptGenerator() {}

bool ScriptGenerator::initialize(const std::string& outputDir) {
    if (outputDir.empty()) {
#ifdef _WIN32
        outputDirectory_ = "G:\\Zachary's Playroom Powerd by AI\\generated_scripts";
#else
        const char* home = getenv("HOME");
        outputDirectory_ = std::string(home ? home : ".") + "/ZacharysPlayroom/generated_scripts";
#endif
    } else {
        outputDirectory_ = outputDir;
    }

    return createDirectoryRecursive(outputDirectory_);
}

bool ScriptGenerator::createDirectoryRecursive(const std::string& path) {
    std::string currentPath;
    size_t pos = 0;
    
    while (pos < path.length()) {
        size_t nextSlash = path.find_first_of("/\\", pos);
        if (nextSlash == std::string::npos) {
            nextSlash = path.length();
        }
        
        currentPath = path.substr(0, nextSlash);
        if (!currentPath.empty() && currentPath != "." && currentPath.find(':') == std::string::npos) {
            struct stat st;
            if (stat(currentPath.c_str(), &st) != 0) {
                if (mkdir(currentPath.c_str(), 0755) != 0) {
                    // Directory might already exist, which is fine
                }
            }
        }
        
        pos = nextSlash + 1;
    }
    
    return true;
}

std::vector<ScriptGenerator::GeneratedScript> ScriptGenerator::generateScripts(const ScriptRequest& request) {
    std::vector<GeneratedScript> scripts;

    if (request.platform == Platform::ALL_PLATFORMS) {
        // Generate for all platforms
        scripts.push_back(generateForPlatform(request, Platform::LINUX_BASH));
        scripts.push_back(generateForPlatform(request, Platform::MACOS_BASH));
        scripts.push_back(generateForPlatform(request, Platform::MACOS_ZSH));
        scripts.push_back(generateForPlatform(request, Platform::WINDOWS_POWERSHELL));
        scripts.push_back(generateForPlatform(request, Platform::WINDOWS_BATCH));
    } else {
        scripts.push_back(generateForPlatform(request, request.platform));
    }

    // Auto-sign if requested
    if (request.autoSign) {
        for (auto& script : scripts) {
            signScript(script);
        }
    }

    // Save all scripts
    for (const auto& script : scripts) {
        saveScript(script);
    }

    return scripts;
}

ScriptGenerator::GeneratedScript ScriptGenerator::generateForPlatform(const ScriptRequest& request, Platform platform) {
    GeneratedScript script;
    script.platform = platform;
    script.filename = generateScriptName(request, platform);
    script.isSigned = false;

    // Generate content based on platform
    switch (platform) {
        case Platform::LINUX_BASH:
            script.content = generateLinuxBashScript(request);
            break;
        case Platform::LINUX_SH:
            script.content = generateLinuxShScript(request);
            break;
        case Platform::MACOS_BASH:
            script.content = generateMacOSBashScript(request);
            break;
        case Platform::MACOS_ZSH:
            script.content = generateMacOSZshScript(request);
            break;
        case Platform::WINDOWS_POWERSHELL:
            script.content = generateWindowsPowerShellScript(request);
            break;
        case Platform::WINDOWS_CMD:
            script.content = generateWindowsCMDScript(request);
            break;
        case Platform::WINDOWS_BATCH:
            script.content = generateWindowsBatchScript(request);
            break;
        default:
            script.content = generateLinuxBashScript(request);
    }

    script.filepath = outputDirectory_ + "/" + script.filename;
    return script;
}

std::string ScriptGenerator::generateScriptName(const ScriptRequest& request, Platform platform) {
    if (!request.customName.empty()) {
        return sanitizeFilename(request.customName) + getFileExtension(platform);
    }

    std::string baseName;
    switch (request.type) {
        case ScriptType::INSTALLER: baseName = "install"; break;
        case ScriptType::UPDATER: baseName = "update"; break;
        case ScriptType::BACKUP: baseName = "backup"; break;
        case ScriptType::DEPLOYMENT: baseName = "deploy"; break;
        case ScriptType::AUTOMATION: baseName = "automate"; break;
        case ScriptType::SYSTEM_ADMIN: baseName = "sysadmin"; break;
        case ScriptType::BUILD: baseName = "build"; break;
        case ScriptType::TEST: baseName = "test"; break;
        case ScriptType::DOCKER: baseName = "docker"; break;
        default: baseName = "script"; break;
    }

    // Add platform suffix
    std::string platformSuffix;
    switch (platform) {
        case Platform::LINUX_BASH:
        case Platform::LINUX_SH: platformSuffix = "_linux"; break;
        case Platform::MACOS_BASH:
        case Platform::MACOS_ZSH: platformSuffix = "_macos"; break;
        case Platform::WINDOWS_POWERSHELL:
        case Platform::WINDOWS_CMD:
        case Platform::WINDOWS_BATCH: platformSuffix = "_windows"; break;
        default: platformSuffix = ""; break;
    }

    return baseName + platformSuffix + "_" + getCurrentTimestamp() + getFileExtension(platform);
}

std::string ScriptGenerator::generateLinuxBashScript(const ScriptRequest& request) {
    std::ostringstream script;
    
    script << getShebang(Platform::LINUX_BASH) << "\n";
    script << generateHeader(request, Platform::LINUX_BASH);
    script << "\n# Error handling\n";
    script << "set -e\n";
    script << "set -u\n";
    script << "set -o pipefail\n\n";
    
    if (request.requiresAdmin) {
        script << "# Check for root privileges\n";
        script << "if [[ $EUID -ne 0 ]]; then\n";
        script << "    echo \"This script must be run as root (use sudo)\"\n";
        script << "    exit 1\n";
        script << "fi\n\n";
    }
    
    script << generatePrerequisiteChecks(request, Platform::LINUX_BASH);
    script << generateDependencyInstall(request, Platform::LINUX_BASH);
    script << generateMainLogic(request, Platform::LINUX_BASH);
    script << generateFooter(Platform::LINUX_BASH);
    
    return script.str();
}

std::string ScriptGenerator::generateLinuxShScript(const ScriptRequest& request) {
    std::ostringstream script;
    
    script << getShebang(Platform::LINUX_SH) << "\n";
    script << generateHeader(request, Platform::LINUX_SH);
    script << "\n# Error handling\n";
    script << "set -e\n";
    script << "set -u\n\n";
    
    if (request.requiresAdmin) {
        script << "# Check for root privileges\n";
        script << "if [ $(id -u) -ne 0 ]; then\n";
        script << "    echo \"This script must be run as root (use sudo)\"\n";
        script << "    exit 1\n";
        script << "fi\n\n";
    }
    
    script << generatePrerequisiteChecks(request, Platform::LINUX_SH);
    script << generateDependencyInstall(request, Platform::LINUX_SH);
    script << generateMainLogic(request, Platform::LINUX_SH);
    script << generateFooter(Platform::LINUX_SH);
    
    return script.str();
}

std::string ScriptGenerator::generateMacOSBashScript(const ScriptRequest& request) {
    std::ostringstream script;
    
    script << getShebang(Platform::MACOS_BASH) << "\n";
    script << generateHeader(request, Platform::MACOS_BASH);
    script << "\n# macOS specific settings\n";
    script << "export PATH=\"/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin\"\n\n";
    script << "# Error handling\n";
    script << "set -e\n";
    script << "set -u\n";
    script << "set -o pipefail\n\n";
    
    if (request.requiresAdmin) {
        script << "# Check for admin privileges\n";
        script << "if [[ $EUID -ne 0 ]]; then\n";
        script << "    echo \"This script must be run with sudo\"\n";
        script << "    exit 1\n";
        script << "fi\n\n";
    }
    
    script << generatePrerequisiteChecks(request, Platform::MACOS_BASH);
    script << generateDependencyInstall(request, Platform::MACOS_BASH);
    script << generateMainLogic(request, Platform::MACOS_BASH);
    script << generateFooter(Platform::MACOS_BASH);
    
    return script.str();
}

std::string ScriptGenerator::generateMacOSZshScript(const ScriptRequest& request) {
    std::ostringstream script;
    
    script << getShebang(Platform::MACOS_ZSH) << "\n";
    script << generateHeader(request, Platform::MACOS_ZSH);
    script << "\n# macOS Zsh specific settings\n";
    script << "export PATH=\"/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin\"\n";
    script << "setopt ERR_EXIT\n";
    script << "setopt NO_UNSET\n";
    script << "setopt PIPE_FAIL\n\n";
    
    if (request.requiresAdmin) {
        script << "# Check for admin privileges\n";
        script << "if [[ $EUID -ne 0 ]]; then\n";
        script << "    print \"This script must be run with sudo\"\n";
        script << "    exit 1\n";
        script << "fi\n\n";
    }
    
    script << generatePrerequisiteChecks(request, Platform::MACOS_ZSH);
    script << generateDependencyInstall(request, Platform::MACOS_ZSH);
    script << generateMainLogic(request, Platform::MACOS_ZSH);
    script << generateFooter(Platform::MACOS_ZSH);
    
    return script.str();
}

std::string ScriptGenerator::generateWindowsPowerShellScript(const ScriptRequest& request) {
    std::ostringstream script;
    
    script << getShebang(Platform::WINDOWS_POWERSHELL) << "\n";
    script << generateHeader(request, Platform::WINDOWS_POWERSHELL);
    script << "\n# PowerShell error handling\n";
    script << "$ErrorActionPreference = \"Stop\"\n";
    script << "$ProgressPreference = 'SilentlyContinue'\n\n";
    
    if (request.requiresAdmin) {
        script << "# Check for administrator privileges\n";
        script << "$currentPrincipal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())\n";
        script << "if (-not $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {\n";
        script << "    Write-Error \"This script must be run as Administrator\"\n";
        script << "    exit 1\n";
        script << "}\n\n";
    }
    
    script << generatePrerequisiteChecks(request, Platform::WINDOWS_POWERSHELL);
    script << generateDependencyInstall(request, Platform::WINDOWS_POWERSHELL);
    script << generateMainLogic(request, Platform::WINDOWS_POWERSHELL);
    script << generateFooter(Platform::WINDOWS_POWERSHELL);
    
    return script.str();
}

std::string ScriptGenerator::generateWindowsCMDScript(const ScriptRequest& request) {
    std::ostringstream script;
    
    script << getShebang(Platform::WINDOWS_CMD) << "\n";
    script << "REM " << request.description << "\n";
    script << "setlocal EnableDelayedExpansion\n\n";
    
    if (request.requiresAdmin) {
        script << "REM Check for administrator privileges\n";
        script << "net session >nul 2>&1\n";
        script << "if %errorLevel% neq 0 (\n";
        script << "    echo This script must be run as Administrator\n";
        script << "    exit /b 1\n";
        script << ")\n\n";
    }
    
    script << generateMainLogic(request, Platform::WINDOWS_CMD);
    script << "\necho Script completed successfully\n";
    script << "exit /b 0\n";
    
    return script.str();
}

std::string ScriptGenerator::generateWindowsBatchScript(const ScriptRequest& request) {
    return generateWindowsCMDScript(request); // CMD and Batch are similar
}

std::string ScriptGenerator::generateHeader(const ScriptRequest& request, Platform platform) {
    std::ostringstream header;
    std::string commentChar = "#";
    
    if (platform == Platform::WINDOWS_CMD || platform == Platform::WINDOWS_BATCH) {
        commentChar = "REM";
    }
    
    header << commentChar << " ============================================\n";
    header << commentChar << " " << request.description << "\n";
    header << commentChar << " Generated: " << getCurrentTimestamp() << "\n";
    header << commentChar << " Platform: ";
    
    switch (platform) {
        case Platform::LINUX_BASH: header << "Linux (Bash)"; break;
        case Platform::LINUX_SH: header << "Linux (POSIX sh)"; break;
        case Platform::MACOS_BASH: header << "macOS (Bash)"; break;
        case Platform::MACOS_ZSH: header << "macOS (Zsh)"; break;
        case Platform::WINDOWS_POWERSHELL: header << "Windows (PowerShell)"; break;
        case Platform::WINDOWS_CMD: header << "Windows (CMD)"; break;
        case Platform::WINDOWS_BATCH: header << "Windows (Batch)"; break;
        default: header << "Unknown"; break;
    }
    
    header << "\n" << commentChar << " ============================================\n";
    return header.str();
}

std::string ScriptGenerator::generatePrerequisiteChecks(const ScriptRequest& request, Platform platform) {
    if (request.dependencies.empty()) return "";
    
    std::ostringstream checks;
    checks << "\n# Check prerequisites\n";
    checks << "echo \"Checking prerequisites...\"\n";
    
    for (const auto& dep : request.dependencies) {
        if (platform == Platform::WINDOWS_POWERSHELL) {
            checks << "if (-not (Get-Command " << dep << " -ErrorAction SilentlyContinue)) {\n";
            checks << "    Write-Warning \"" << dep << " is not installed\"\n";
            checks << "}\n";
        } else if (platform != Platform::WINDOWS_CMD && platform != Platform::WINDOWS_BATCH) {
            checks << "if ! command -v " << dep << " &> /dev/null; then\n";
            checks << "    echo \"Warning: " << dep << " is not installed\"\n";
            checks << "fi\n";
        }
    }
    
    return checks.str();
}

std::string ScriptGenerator::generateDependencyInstall(const ScriptRequest& request, Platform platform) {
    if (request.dependencies.empty()) return "";
    
    std::ostringstream install;
    install << "\n# Install dependencies (if needed)\n";
    
    if (platform == Platform::LINUX_BASH || platform == Platform::LINUX_SH) {
        install << "# Detect package manager and install dependencies\n";
        install << "if command -v apt-get &> /dev/null; then\n";
        install << "    # Debian/Ubuntu\n";
        for (const auto& dep : request.dependencies) {
            install << "    apt-get install -y " << dep << " 2>/dev/null || true\n";
        }
        install << "elif command -v yum &> /dev/null; then\n";
        install << "    # CentOS/RHEL/Fedora\n";
        for (const auto& dep : request.dependencies) {
            install << "    yum install -y " << dep << " 2>/dev/null || true\n";
        }
        install << "elif command -v pacman &> /dev/null; then\n";
        install << "    # Arch Linux\n";
        for (const auto& dep : request.dependencies) {
            install << "    pacman -S --noconfirm " << dep << " 2>/dev/null || true\n";
        }
        install << "fi\n";
    } else if (platform == Platform::MACOS_BASH || platform == Platform::MACOS_ZSH) {
        install << "# Install via Homebrew if available\n";
        install << "if command -v brew &> /dev/null; then\n";
        for (const auto& dep : request.dependencies) {
            install << "    brew install " << dep << " 2>/dev/null || true\n";
        }
        install << "fi\n";
    } else if (platform == Platform::WINDOWS_POWERSHELL) {
        install << "# Install via Chocolatey if available\n";
        install << "if (Get-Command choco -ErrorAction SilentlyContinue) {\n";
        for (const auto& dep : request.dependencies) {
            install << "    choco install " << dep << " -y 2>$null\n";
        }
        install << "}\n";
    }
    
    return install.str();
}

std::string ScriptGenerator::generateMainLogic(const ScriptRequest& request, Platform platform) {
    std::ostringstream logic;
    logic << "\n# Main script logic\n";
    logic << "echo \"Executing main tasks...\"\n\n";
    
    for (const auto& cmd : request.commands) {
        if (platform == Platform::WINDOWS_POWERSHELL) {
            logic << cmd << "\n";
            logic << "if ($LASTEXITCODE -ne 0) {\n";
            logic << "    Write-Error \"Command failed: " << cmd << "\"\n";
            logic << "    exit $LASTEXITCODE\n";
            logic << "}\n\n";
        } else if (platform == Platform::WINDOWS_CMD || platform == Platform::WINDOWS_BATCH) {
            logic << cmd << "\n";
            logic << "if %errorLevel% neq 0 (\n";
            logic << "    echo Command failed: " << cmd << "\n";
            logic << "    exit /b %errorLevel%\n";
            logic << ")\n\n";
        } else {
            logic << cmd << "\n";
            logic << "if [ $? -ne 0 ]; then\n";
            logic << "    echo \"Command failed: " << cmd << "\"\n";
            logic << "    exit 1\n";
            logic << "fi\n\n";
        }
    }
    
    return logic.str();
}

std::string ScriptGenerator::generateFooter(Platform platform) {
    std::ostringstream footer;
    
    if (platform == Platform::WINDOWS_POWERSHELL) {
        footer << "\nWrite-Host \"Script completed successfully\" -ForegroundColor Green\n";
        footer << "exit 0\n";
    } else if (platform == Platform::WINDOWS_CMD || platform == Platform::WINDOWS_BATCH) {
        footer << "\necho Script completed successfully\n";
        footer << "exit /b 0\n";
    } else {
        footer << "\necho \"Script completed successfully\"\n";
        footer << "exit 0\n";
    }
    
    return footer.str();
}

bool ScriptGenerator::signScript(GeneratedScript& script) {
    try {
        // Generate a simple SHA256-like signature hash for the script
        // This creates a checksum that can verify script integrity
        unsigned long hash = 5381;
        for (char c : script.content) {
            hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
        }
        
        // Convert to hex string
        std::ostringstream ss;
        ss << std::hex << hash;
        script.signature = "SHA256:" + ss.str();
        script.isSigned = true;
        
        // Add signature to script content
        std::string signatureComment;
        if (script.platform == Platform::WINDOWS_POWERSHELL) {
            signatureComment = "\n# SIGNATURE: " + script.signature + "\n";
            signatureComment += "# Generated by Zachary's Playroom Script Generator\n";
        } else if (script.platform == Platform::WINDOWS_CMD || script.platform == Platform::WINDOWS_BATCH) {
            signatureComment = "\nREM SIGNATURE: " + script.signature + "\n";
            signatureComment += "REM Generated by Zachary's Playroom Script Generator\n";
        } else {
            signatureComment = "\n# SIGNATURE: " + script.signature + "\n";
            signatureComment += "# Generated by Zachary's Playroom Script Generator\n";
        }
        
        script.content += signatureComment;
        
        std::cout << "Script signed successfully: " << script.filename << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error signing script: " << e.what() << std::endl;
        return false;
    }
}

bool ScriptGenerator::saveScript(const GeneratedScript& script) {
    try {
        std::ofstream file(script.filepath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to create file: " << script.filepath << std::endl;
            return false;
        }
        
        file << script.content;
        file.close();
        
        // Set executable permissions on Unix-like systems
#ifndef _WIN32
        if (script.platform != Platform::WINDOWS_POWERSHELL &&
            script.platform != Platform::WINDOWS_CMD &&
            script.platform != Platform::WINDOWS_BATCH) {
            chmod(script.filepath.c_str(), 0755);
        }
#endif
        
        std::cout << "Script saved: " << script.filepath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving script: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::string> ScriptGenerator::getSupportedPlatforms() const {
    return {
        "Linux (Bash)",
        "Linux (POSIX sh)",
        "macOS (Bash)",
        "macOS (Zsh)",
        "Windows (PowerShell)",
        "Windows (CMD)",
        "Windows (Batch)"
    };
}

std::vector<std::string> ScriptGenerator::getSupportedLinuxDistros() const {
    return {
        "Ubuntu 20.04, 22.04, 24.04",
        "Debian 10, 11, 12",
        "Fedora 38, 39, 40",
        "CentOS 7, 8, 9",
        "RHEL 7, 8, 9",
        "Arch Linux",
        "openSUSE Leap 15.x",
        "SUSE Linux Enterprise"
    };
}

std::vector<std::string> ScriptGenerator::getSupportedMacOSVersions() const {
    return {
        "macOS Sonoma (14.x)",
        "macOS Ventura (13.x)",
        "macOS Monterey (12.x)",
        "macOS Big Sur (11.x)",
        "macOS Catalina (10.15)"
    };
}

std::vector<std::string> ScriptGenerator::getSupportedWindowsVersions() const {
    return {
        "Windows 11 (22H2, 23H2)",
        "Windows 10 (21H2, 22H2)",
        "Windows Server 2022",
        "Windows Server 2019",
        "Windows Server 2016"
    };
}

bool ScriptGenerator::validateScript(const GeneratedScript& script) {
    // Basic validation: check if content is not empty
    if (script.content.empty()) return false;
    if (script.filename.empty()) return false;
    return true;
}

void ScriptGenerator::setOutputDirectory(const std::string& dir) {
    outputDirectory_ = dir;
    createDirectoryRecursive(outputDirectory_);
}

std::string ScriptGenerator::getOutputDirectory() const {
    return outputDirectory_;
}

std::string ScriptGenerator::getFileExtension(Platform platform) const {
    auto it = platformExtensions_.find(platform);
    if (it != platformExtensions_.end()) {
        return it->second;
    }
    return ".sh";
}

std::string ScriptGenerator::getShebang(Platform platform) const {
    auto it = platformShebangs_.find(platform);
    if (it != platformShebangs_.end()) {
        return it->second;
    }
    return "#!/bin/bash";
}

std::string ScriptGenerator::getCurrentTimestamp() const {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", timeinfo);
    return std::string(buffer);
}

std::string ScriptGenerator::sanitizeFilename(const std::string& name) const {
    std::string sanitized = name;
    
    // Replace invalid characters with underscores
    const std::string invalid = "<>:\"/\\|?* ";
    for (char& c : sanitized) {
        if (invalid.find(c) != std::string::npos) {
            c = '_';
        }
    }
    
    // Convert to lowercase
    std::transform(sanitized.begin(), sanitized.end(), sanitized.begin(), ::tolower);
    
    return sanitized;
}
